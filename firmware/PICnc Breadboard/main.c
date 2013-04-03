/*    Copyright (C) 2012 GP Orcullo
 *
 *    This file is part of PiCnc.
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <plib.h>
#include "hardware.h"
#include "stepgen.h"

#pragma config POSCMOD = OFF		/* Primary Oscillator disabled */
#pragma config FNOSC = FRCPLL		/* Fast RC Osc w/Div-by-N */
#pragma config FPLLODIV = DIV_2		/* PLL configured for 40MHz clock */
#pragma config FPLLMUL = MUL_20
#pragma config FPLLIDIV = DIV_2
#pragma config FPBDIV = DIV_1		/* Peripheral Clock Divisor */
#pragma config IESO = ON		/* Internal/External Switch Over disabled */
#pragma config FSOSCEN = OFF		/* Secondary Oscillator disabled */
#pragma config CP = OFF			/* Code Protect Disabled */
#pragma config FWDTEN = ON		/* Watchdog Timer Enable */
#pragma config WDTPS = PS4096		/* Watchdog Timer Postscaler */
#pragma config PMDL1WAY = OFF		/* Allow multiple PM configurations */
#pragma config IOL1WAY = OFF		/* Allow multiple PPS configurations */

#define BASEFREQ			80000
#define CORE_TICK_RATE	        	(SYS_FREQ/2/BASEFREQ)
#define CORE_DIVIDER			(BASEFREQ/CLOCK_CONF_SECOND)

#define SPIBUFSIZE			32		/* BCM2835 SPI buffer size, reduced by half 
							   since SPI speed is slower */
#define BUFSIZE				(SPIBUFSIZE/4)

#define ENABLE_WATCHDOG

static volatile uint32_t rxBuf[BUFSIZE], txBuf[BUFSIZE];
static volatile int spi_data_ready = 0;

void map_peripherals()
{
	/* unlock PPS sequence */
	SYSKEY = 0x0;			/* make sure it is locked */
	SYSKEY = 0xAA996655;		/* Key 1 */
	SYSKEY = 0x556699AA;		/* Key 2 */
	CFGCONbits.IOLOCK=0;		/* now it is unlocked */

	configure_PPS();

	/* lock PPS sequence */
	CFGCONbits.IOLOCK=1;		/* now it is unlocked */
	SYSKEY = 0x0;			/* lock register access */
}

void init_io_ports()
{
	/* disable all analog pins */
	ANSELA = 0x0;
	ANSELB = 0x0;

	/* setup inputs */
	configure_inputs();

	/* LED */
	LED0_TRIS = 0;
	LED0_IO = 0;

	/* data ready, active low */
	RDY_TRIS = 0;
	RDY_IO = 1;

	/* data request, active low, pull-up enabled */
	REQ_TRIS = 1;
	REQ_CNPU_Enable();

	/* configure step and dir pins as outputs */
	configure_stepdir();
}

void init_spi()
{
	/* configure SPI */
	SpiChnEnable(SPICHAN, 0);

	/* Slave mode, CKE, 8 bits, enhanced buffer, interrupt on rx */
	SpiChnConfigure(SPICHAN, SPI_CONFIG_SLVEN | SPI_CONFIG_CKE_REV |
	        SPI_CONFIG_ENHBUF | SPI_CONFIG_RBF_NOT_EMPTY);

	/* start SPI */
	SpiChnEnable(SPICHAN, 1);

	/* preload buffer */
	SPI2BUF = 0x88;
}

void init_dma()
{
	/* open and configure the DMA channels
	     DMA 0 is for SPI -> buffer, this is the master channel, auto enabled
	     DMA 1 is for buffer -> SPI, this channel is chained to DMA 0 */
	DmaChnOpen(DMA_CHANNEL0, DMA_CHN_PRI3, DMA_OPEN_AUTO);
	DmaChnOpen(DMA_CHANNEL1, DMA_CHN_PRI3, DMA_CONFIG_DET_EN | DMA_OPEN_CHAIN_HI);

	/* DMA channels trigger on SPI RX, buffer not empty signal */
	DmaChnSetEventControl(DMA_CHANNEL0, DMA_EV_START_IRQ(_SPI2_RX_IRQ));
	DmaChnSetEventControl(DMA_CHANNEL1, DMA_EV_START_IRQ(_SPI2_RX_IRQ));

	/* transfer 8bits at a time */
	DmaChnSetTxfer(DMA_CHANNEL0, (void *)&SPI2BUF, (void *)rxBuf, 1, sizeof(rxBuf), 1);
	DmaChnSetTxfer(DMA_CHANNEL1, (void *)txBuf, (void *)&SPI2BUF, sizeof(txBuf), 1, 1);

	/* enable the transfer done interrupt */
	DmaChnSetEvEnableFlags(0, DMA_EV_BLOCK_DONE);

	/* set interrupt priority */
	INTSetVectorPriority(INT_VECTOR_DMA(DMA_CHANNEL0), INT_PRIORITY_LEVEL_5);
	INTSetVectorSubPriority(INT_VECTOR_DMA(DMA_CHANNEL0), INT_SUB_PRIORITY_LEVEL_3);

	/* enable DMA interrupts */
	INTEnable(INT_SOURCE_DMA(DMA_CHANNEL0), INT_ENABLED);

	/* start DMA 0 */
	DmaChnEnable(0);
}

int main(void)
{
	int spi_timeout, i;
	static uint32_t x = 0;

	/* Disable JTAG port so we get our I/O pins back */
	DDPCONbits.JTAGEN = 0;
	/* Enable optimal performance */
	SYSTEMConfigPerformance(GetSystemClock());
	/* Use 1:1 CPU Core:Peripheral clocks */
	OSCSetPBDIV(OSC_PB_DIV_1);

	/* configure the core timer roll-over rate */
	OpenCoreTimer(CORE_TICK_RATE);

	/* set up the core timer interrupt */
	mConfigIntCoreTimer((CT_INT_ON | CT_INT_PRIOR_6 | CT_INT_SUB_PRIOR_0));

	/* enable multi vector interrupts */
	INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
	INTEnableInterrupts();

	map_peripherals();
	init_io_ports();
	init_spi();
	init_dma();

	stepgen_reset();
	spi_data_ready = 0;
	spi_timeout = 0;

#if defined(ENABLE_WATCHDOG)
	WDTCONSET = 0x8000;
#endif

	/* main loop */
	while (1) {

		/* process incoming data request,
		   transfer valid data to txBuf */
		if (!REQ_IO_IN) {
			stepgen_get_position((void *)&txBuf[1]);
			
			/* read inputs */
			txBuf[5] = PORTB & 0x3E0;

			/* sanity check */
			txBuf[0] = rxBuf[0];
			txBuf[BUFSIZE-1] = rxBuf[0] >> 8;

			/* reset spi_timeout */
			spi_timeout = 0;

			RDY_IO_0;	/* the ready line is active low */
		} else {
			RDY_IO_1;
		}

		/* process received data */
		if (spi_data_ready) {
			spi_data_ready = 0;

			/* the first byte received is a command byte */
			switch (rxBuf[0]) {
			case 0x5453523E:	/* >RST */
				stepgen_reset();
				break;
			case 0x444D433E:	/* >CMD */
				stepgen_update_input((const void *)&rxBuf[1]);
				break;
			}
		}

		/* shutdown stepgen if no activity */
		if (spi_timeout++ > 20000L) {
			spi_timeout = 20000L;
			stepgen_reset();
		}

		/* blink onboard led */
		if (x++ == 250000L) {
			x = 0;
			LED0_IO ^= 1;
		}
#if defined(ENABLE_WATCHDOG)
		/* keep alive */
		WDTCONSET = 0x01;
#endif
	}
	return 0;
}

void __ISR(_CORE_TIMER_VECTOR, ipl6) CoreTimerHandler(void)
{
	/* update the period */
	UpdateCoreTimer(CORE_TICK_RATE);

	/* do repetitive tasks here */
	stepgen();

	/* clear the interrupt flag */
	mCTClearIntFlag();
}

void __ISR(_DMA0_VECTOR, ipl5) DmaHandler0(void)
{
	int	evFlags;

	/* acknowledge interrupt */
	INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL0));

	/* confirm block transfer done */
	evFlags=DmaChnGetEvFlags(DMA_CHANNEL0);
	if (evFlags & DMA_EV_BLOCK_DONE) {
		DmaChnClrEvFlags(DMA_CHANNEL0, DMA_EV_BLOCK_DONE);

		spi_data_ready = 1;
	}
}



/*    Copyright (C) 2012 GP Orcullo
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

#define SPIBUFSIZE			32	/* BCM2835 SPI buffer size for 
						   PIC32MX150F128B */
#define BUFSIZE				(SPIBUFSIZE/4)

#define ENABLE_WATCHDOG

static volatile uint32_t rxBuf[BUFSIZE], txBuf[BUFSIZE];
static volatile int spi_data_ready;

static void map_peripherals()
{
	/* unlock PPS sequence */
	SYSKEY = 0x0;			/* make sure it is locked */
	SYSKEY = 0xAA996655;		/* Key 1 */
	SYSKEY = 0x556699AA;		/* Key 2 */
	CFGCONbits.IOLOCK=0;		/* now it is unlocked */

	/* map SPI pins */
	PPSInput(3, SDI2, RPB13);	/* MOSI */
	PPSOutput(2, RPA1, SDO2);	/* MISO */

	/* lock PPS sequence */
	CFGCONbits.IOLOCK=1;		/* now it is locked */
	SYSKEY = 0x0;			/* lock register access */
}

static void init_io_ports()
{
	/* disable all analog pins */
	ANSELA = 0x0;
	ANSELB = 0x0;

	/* configure inputs */
	TRISBSET = BIT_1 | BIT_5 | BIT_6 | BIT_7 |
		   BIT_8 | BIT_9 | BIT_13 | BIT_15;

	/* configure_outputs */
	TRISACLR = BIT_0 | BIT_1 | BIT_2  | BIT_3  | BIT_4;
	TRISBCLR = BIT_0 | BIT_2 | BIT_3 | BIT_4 |
		   BIT_10 | BIT_11 | BIT_12 | BIT_14;

	/* enable pull-ups on inputs */
	ConfigCNBPullups(CNB1_PULLUP_ENABLE | CNB5_PULLUP_ENABLE |
			 CNB6_PULLUP_ENABLE | CNB7_PULLUP_ENABLE |
			 CNB8_PULLUP_ENABLE | CNB9_PULLUP_ENABLE |
			 CNB13_PULLUP_ENABLE | CNB15_PULLUP_ENABLE);

	/* data ready, active low */
	RDY_HI;
}

static void init_spi()
{
	int i;

	SPI2CON = 0;		/* stop SPI 2, set Slave mode, 8 bits, std buffer */
	i = SPI2BUF;		/* clear rcv buffer */
	SPI2CON = 1<<8 | 0<<6;	/* Clock Edge */
	SPI2CONSET = 1<<15;	/* start SPI 2 */
}

static void init_dma()
{
	/* open and configure the DMA channels
	     DMA 0 is for SPI -> buffer, this is the master channel, auto enabled
	     DMA 1 is for buffer -> SPI, this channel is chained to DMA 0 */
	DmaChnOpen(DMA_CHANNEL0, DMA_CHN_PRI3, DMA_OPEN_AUTO);
	DmaChnOpen(DMA_CHANNEL1, DMA_CHN_PRI0, DMA_OPEN_DEFAULT);

	/* DMA channels trigger on SPI RX, buffer not empty signal */
	DmaChnSetEventControl(DMA_CHANNEL0, DMA_EV_START_IRQ(_SPI2_RX_IRQ));
	DmaChnSetEventControl(DMA_CHANNEL1, DMA_EV_START_IRQ(_SPI2_TX_IRQ));

	/* transfer 8bits at a time */
	DmaChnSetTxfer(DMA_CHANNEL0, (void *)&SPI2BUF, (void *)rxBuf, 1, SPIBUFSIZE, 1);
	DmaChnSetTxfer(DMA_CHANNEL1, (void *)txBuf, (void *)&SPI2BUF, SPIBUFSIZE, 1, 1);

	/* start DMA 0 */
	DmaChnEnable(0);
	DmaChnEnable(1);
}

static inline uint32_t read_inputs()
{
	uint32_t x;
	
	x  = (INP0_IN ? 1 : 0) << 0;
	x |= (INP1_IN ? 1 : 0) << 1;
	x |= (INP2_IN ? 1 : 0) << 2;
	x |= (INP3_IN ? 1 : 0) << 3;
	x |= (INP4_IN ? 1 : 0) << 4;

	return x;
}

static inline void update_outputs(uint32_t x)
{
	if (MAXGEN != 4) {
		if (x & (1 << 0))
			OUT0_HI;
		else
			OUT0_LO;

		if (x & (1 << 1))
			OUT1_HI;
		else
			OUT1_LO;
	}

	if (x & (1 << 2))
		OUT2_HI;
	else
		OUT2_LO;

	if (x & (1 << 3))
		OUT3_HI;
	else
		OUT3_LO;
}

void reset_board()
{
	stepgen_reset();
	update_outputs(0);
}

int main(void)
{
	int spi_timeout, spi_reset = 1, i;
	unsigned long counter;

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

	reset_board();
	spi_data_ready = 0;
	spi_timeout = 20000L;
	counter = 0;

#if defined(ENABLE_WATCHDOG)
	WDTCONSET = 0x8000;
#endif

	/* main loop */
	while (1) {
		if (!REQ_IN) { 
			stepgen_get_position((void *)&txBuf[1]);

			/* read inputs */
			txBuf[1+MAXGEN] = read_inputs();

			/* the ready line is active low */
			RDY_LO;
		} else {
			RDY_HI;
		}

		if (spi_data_ready) {
			spi_data_ready = 0;
			
			/* reset spi_timeout */
			spi_timeout = 20000L;

			/* the first byte received is a command byte */
			switch (rxBuf[0]) {
			case 0x5453523E:	/* >RST */
				reset_board();
				break;
			case 0x444D433E:	/* >CMD */
				stepgen_update_input((const void *)&rxBuf[1]);
				update_outputs(rxBuf[1+MAXGEN]);
				break;
			case 0x4746433E:	/* >CFG */
				stepgen_update_stepwidth(rxBuf[1]);
				stepgen_reset();
				break;
			case 0x5453543E:	/* >TST */
				for (i=0; i<BUFSIZE; i++) 
					txBuf[i] = rxBuf[i] ^ ~0;
				break;
			}
		}
		
		if (DCH0INTbits.CHBCIF) {
			DCH0INTCLR = 1<<3;
			
			/* data integrity check */
			txBuf[0] = rxBuf[0] ^ ~0;
			spi_data_ready = 1;

			/* restart rx DMA */
			DmaChnEnable(1);
		} 

		/* shutdown stepgen if no activity */
		if (spi_timeout) {
			spi_timeout--;
			spi_reset = 1;
		} else {
			if (spi_reset) {
				spi_reset = 0;
				/* abort DMA transfer */
				DCH0ECONSET=BIT_6;
				DCH1ECONSET=BIT_6;
			
				init_spi();
				init_dma();
			}
			
			reset_board();
		}

		/* blink onboard led */
		if (!(counter++ % (spi_timeout ? 0x10000 : 0x20000))) { 
			LED_TOGGLE;
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



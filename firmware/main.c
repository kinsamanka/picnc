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

#pragma config POSCMOD = XT		/* Primary Oscillator XT mode */
#pragma config FNOSC = PRIPLL		/* Primary Osc w/PLL */
#pragma config FPLLODIV = DIV_1		/* PLL configured for 80MHz clock */
#pragma config FPLLMUL = MUL_20
#pragma config FPLLIDIV = DIV_2
#pragma config FPBDIV = DIV_1		/* Peripheral Clock Divisor */
#pragma config IESO = ON		/* Internal/External Switch Over disabled */
#pragma config FSOSCEN = OFF		/* Secondary Oscillator disabled */
#pragma config CP = OFF			/* Code Protect Disabled */
#pragma config FWDTEN = OFF		/* Watchdog Timer Disable */
#pragma config WDTPS = PS4096		/* Watchdog Timer Postscaler */
#pragma config FVBUSONIO = OFF		/* VBUSON pin is GPIO */
#pragma config FUSBIDIO = OFF		/* USBID pin is GPIO */

#define BASEFREQ			160000
#define CORE_TICK_RATE	        	(SYS_FREQ/2/BASEFREQ)
#define CORE_DIVIDER			(BASEFREQ/CLOCK_CONF_SECOND)

#define SPIBUFSIZE			32
#define BUFSIZE				(SPIBUFSIZE/4)

#define ENABLE_WATCHDOG

static volatile uint32_t rxBuf[BUFSIZE], txBuf[BUFSIZE];
static volatile int spi_data_ready;

static void init_io_ports()
{
	U1PWRCbits.USUSPEND = 1;
	U1PWRCbits.USBPWR = 0;

	/* disable all analog pins except for pins 2-0 */
	AD1PCFG = 0xFFF8;

	/* configure inputs */
	TRISBSET = 0xFFFF;
	TRISGSET = BIT_2 | BIT_3 | BIT_6 | BIT_7;

	/* configure_outputs */
	TRISCCLR = BIT_13 | BIT_14;
	TRISDCLR = 0xFFF;
	TRISECLR = 0xFF;
	TRISFCLR = BIT_0 | BIT_1 | BIT_3;
	TRISGCLR = BIT_8;

	/* enable open drain on step and dir outputs*/
	ODCESET = BIT_7 | BIT_6 | BIT_5 | BIT_4 | BIT_3 | BIT_2 | BIT_1 | BIT_0;
	
	/* enable open drain on all output pins */
	ODCDSET = BIT_7 | BIT_6 | BIT_5 | BIT_4 | BIT_3 | BIT_2 | BIT_1 | BIT_0;
	ODCDSET = BIT_11 | BIT_10 | BIT_9 | BIT_8;
	ODCFSET = BIT_3 | BIT_1 | BIT_0;

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

/* PWM is using OC1, OC2, OC3 and Timer2 */
static inline void configure_pwm()
{
	OC1CON = 0x0000;	/* disable OCx */
	OC2CON = 0x0000;
	OC3CON = 0x0000;
	OC1R = 0;		/* set output low */
	OC2R = 0;
	OC3R = 0;
	OC1RS = 0;
	OC2RS = 0;
	OC3RS = 0;
	OC1CON = 0x0006;	/* PWM mode, fault pin disabled */
	OC2CON = 0x0006;
	OC3CON = 0x0006;
	T2CONSET = 0x0008;	/* Timer2 32 bit mode */
	PR2 = 0x9C3F;		/* set period, 1kHz */
	T2CONSET = 0x8000;	/* start timer */
	OC1CONSET = 0x8020;	/* enable OCx in 32 bit mode */
	OC2CONSET = 0x8020;
	OC3CONSET = 0x8020;
}

static inline void update_pwm_period(uint32_t val)
{
	PR2 = val;
}

static inline void update_pwm_duty(uint32_t val1, uint32_t val2)
{
	OC1RS = val1 >> 16;
	OC2RS = val1 & 0xFFFF;
	OC3RS = val2 >> 16;
}

static inline uint32_t read_inputs()
{
	return (PORTB >> 3);
}

static inline void update_outputs(uint32_t val) 
{
	LATDCLR =  PORTD_OUT_MASK & ~(val << 3);
	LATDSET =  PORTD_OUT_MASK &  (val << 3);
	val = val >> 9;
	if (val && 0b100) 
		val = 0b1000 | (val && 0b11);
	else
		val = val && 0b11;
	LATFCLR =  PORTF_OUT_MASK & ~(val);
	LATFSET =  PORTF_OUT_MASK &  (val);
}

void reset_board()
{
	stepgen_reset();
	update_outputs(0);
	update_pwm_duty(0,0);
}

int main(void)
{
	int spi_timeout, i;
	unsigned long counter;

	BMXCONbits.BMXARB = 0x02;
	
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

	init_io_ports();
	configure_pwm();
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
				update_pwm_duty(rxBuf[2+MAXGEN],rxBuf[3+MAXGEN]);
				break;
			case 0x4746433E:	/* >CFG */
				stepgen_update_stepwidth(rxBuf[1]);
				update_pwm_period(rxBuf[2]);
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
		if (spi_timeout)
			spi_timeout--;
		else
			reset_board();

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



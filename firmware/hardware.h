/*    Copyright (C) 2013 GP Orcullo
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

#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#define SYS_FREQ		(40000000ul)    /* 40 kHz */
#define GetSystemClock()	(SYS_FREQ)
#define	GetPeripheralClock()	(GetSystemClock())
#define	GetInstructionClock()	(GetSystemClock())

#define SPICHAN			2

/*    PORT USAGE
 *
 *	Pin	Port	Dir	Signal
 *
 *	2	RA0	OUT	Status LED
 *	3	RA1	OUT	MISO
 *	9	RA2	OUT	DIR_X
 *	10	RA3	OUT	STEP_X
 *	12	RA4	OUT	STEP_Z
 *	4	RB0	OUT	DATA READY
 *	6	RB2	OUT	STEP_Y
 *	7	RB3	OUT	DIR_Y
 *	11	RB4	OUT	DIR_Z
 *	21	RB10	OUT	PWM
 *	22	RB11	OUT	OUTPUT 0
 *	23	RB12	OUT	OUTPUT 1
 *	25	RB14	OUT	OUTPUT 2
 *
 *	5	RB1	IN	DATA REQUEST
 *	14	RB5	IN	INPUT 0
 *	15	RB6	IN	INPUT 1
 *	16	RB7	IN	INPUT 2
 *	17	RB8	IN	INPUT 3
 *	18	RB9	IN	INPUT 4
 *	24	RB13	IN	MOSI
 *	26	RB15	IN	SCLK
 *
 */

#define LED0_BLINK		(LATAINV = BIT_0)
#define REQ_IO_IN		(PORTBbits.RB1)
#define RDY_IO_LO		(LATBCLR = BIT_0)
#define RDY_IO_HI		(LATBSET = BIT_0)

/* enable pull-ups on REQ_IO_IN line */
#define REQ_CNPU_Enable()	ConfigCNBPullups(CNB1_PULLUP_ENABLE)

/* map SPI and PWM pins */
static inline void configure_PPS() {
	PPSInput(3, SDI2, RPB13);	/* MOSI */
	PPSOutput(2, RPA1, SDO2);	/* MISO */
	PPSOutput(4, RPB10, OC3);	/* PWM */
}


static inline void configure_inputs() {
	TRISBSET = BIT_15 | BIT_13 | BIT_9 | BIT_8 |
		   BIT_7  | BIT_6  | BIT_5 | BIT_1 ;
}

static inline void configure_outputs() {
	TRISBCLR = BIT_14 | BIT_12 | BIT_11 | BIT_10 |
		   BIT_4  | BIT_3  | BIT_2  | BIT_0 ;
	TRISACLR = BIT_4  | BIT_3  | BIT_2  | BIT_1  | BIT_0;
}

/* PWM is using OC3 and Timer2, note output pin is inverted */
static inline void configure_pwm() {
	OC3CON = 0x0000;	/* disable OC3 */
	OC3R = ~0;		/* set output high */
	OC3RS = ~0;
	OC3CON = 0x0006;	/* PWM mode, fault pin disabled */
	T2CONSET = 0x0008;	/* Timer2 32 bit mode */
	PR2 = 0x9C3F;		/* set period, 1kHz */
	T2CONSET = 0x8000;	/* start timer */
	OC3CONSET = 0x8020;	/* enable OC3 in 32 bit mode */
}

static inline void update_pwm_period(uint32_t val) {
	PR2 = val;
}

static inline void update_pwm_duty(uint32_t val) {
	OC3RS = val;
}

/* Note the outputs are inverted */

#define PORTB_OUT_MASK	(BIT_14 | BIT_12 | BIT_11)
static inline void update_outputs(uint32_t val) {
	LATBCLR =  PORTB_OUT_MASK &  (val);
	LATBSET =  PORTB_OUT_MASK & ~(val);
}

#define STEPHI_X		(LATACLR = BIT_3)
#define STEPLO_X		(LATASET = BIT_3)
#define DIR_HI_X		(LATACLR = BIT_2)
#define DIR_LO_X		(LATASET = BIT_2)

#define STEPHI_Y		(LATBCLR = BIT_2)
#define STEPLO_Y		(LATBSET = BIT_2)
#define DIR_HI_Y		(LATBCLR = BIT_3)
#define DIR_LO_Y		(LATBSET = BIT_3)

#define STEPHI_Z		(LATACLR = BIT_4)
#define STEPLO_Z		(LATASET = BIT_4)
#define DIR_HI_Z		(LATBCLR = BIT_4)
#define DIR_LO_Z		(LATBSET = BIT_4)

#endif /* __HARDWARE_H__ */
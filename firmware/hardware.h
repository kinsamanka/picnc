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
 *	9	RA2	OUT	OUTPUT 0
 *	10	RA3	OUT	PWM
 *	4	RB0	OUT	DATA READY
 *	5	RB1	OUT	MISO
 *	14	RB5	OUT	Status LED
 *	15	RB6	OUT	DIR_A
 *	16	RB7	OUT	STEP_A
 *	17	RB8	OUT	DIR_Z
 *	18	RB9	OUT	STEP_Z
 *	21	RB10	OUT	DIR_Y
 *	22	RB11	OUT	STEP_Y
 *	23	RB12	OUT	DIR_X
 *	24	RB13	OUT	STEP_X
 *
 *	3	RA1	IN	DATA REQUEST
 *	6	RB2	IN	MOSI
 *	26	RB15	IN	SCLK
 *
 *	2	RA0	IN	unused
 *	12	RA4	IN	unused
 *	7	RB3	IN	unused
 *	11	RB4	IN	unused
 *	25	RB14	IN	unused
 *
 */
 
#define LED0_BLINK		(LATBINV = BIT_5)
#define REQ_IO_IN		(PORTAbits.RA1)
#define RDY_IO_LO		(LATBCLR = BIT_0)
#define RDY_IO_HI		(LATBSET = BIT_0)

/* enable pull-ups on REQ_IO_IN line */
#define REQ_CNPU_Enable()							\
	do {									\
		ConfigCNBPullups(CNB1_PULLUP_ENABLE);				\
	} while (0)

/* map SPI and PWM pins */
#define configure_PPS()								\
	do {									\
		PPSInput(3, SDI2, RPB2);					\
		PPSOutput(2, RPB1, SDO2);					\
		PPSOutput(4, RPA3, OC3);					\
	} while (0)

#define configure_inputs()							\
	do {									\
		TRISASET = BIT_0  | BIT_1  | BIT_4 ;				\
		TRISBSET = BIT_2  | BIT_3  | BIT_4  |				\
			   BIT_14 | BIT_15 ;					\
	} while (0)
 
#define configure_outputs()							\
	do {									\
		TRISACLR = BIT_2  | BIT_3  ;					\
		TRISBCLR = BIT_0  | BIT_1  | BIT_5  | BIT_6  |			\
			   BIT_7  | BIT_8  | BIT_9  | BIT_10 |			\
			   BIT_11 | BIT_12 | BIT_13 ;				\
	} while (0)

/* PWM is using OC3 and Timer2, note output pin is inverted */
#define configure_pwm()								\
	do {									\
		OC3CON = 0x0000;	/* disable OC3 */			\
		OC3R = ~0;		/* set output high */			\
		OC3RS = ~0;							\
		OC3CON = 0x0006;	/* PWM mode, fault pin disabled */	\
		T2CONSET = 0x0008;	/* Timer2 32 bit mode */		\
		PR2 = 0x9C3F;		/* set period, 1kHz */			\
		T2CONSET = 0x8000;	/* start timer */			\
		OC3CONSET = 0x8020;	/* enable OC3 in 32 bit mode */		\
	} while (0)

#define update_pwm_period(val)								\
	do {									\
		PR2 = (val);							\
	} while (0)

#define update_pwm_duty(val)								\
	do {									\
		OC3RS = (val);							\
	} while (0)

/* Note the outputs are inverted */

#define PORTA_OUT_MASK	(BIT_2)
#define update_outputs(val)							\
	do {									\
		LATACLR =  PORTA_OUT_MASK &  (val);				\
		LATASET =  PORTA_OUT_MASK & ~(val);				\
	} while (0)

#define STEPHI_X		(LATBCLR = BIT_13)
#define STEPLO_X		(LATBSET = BIT_13)
#define DIR_HI_X		(LATBCLR = BIT_12)
#define DIR_LO_X		(LATBSET = BIT_12)

#define STEPHI_Y		(LATBCLR = BIT_11)
#define STEPLO_Y		(LATBSET = BIT_11)
#define DIR_HI_Y		(LATBCLR = BIT_10)
#define DIR_LO_Y		(LATBSET = BIT_10)

#define STEPHI_Z		(LATBCLR = BIT_9)
#define STEPLO_Z		(LATBSET = BIT_9)
#define DIR_HI_Z		(LATBCLR = BIT_8)
#define DIR_LO_Z		(LATBSET = BIT_8)

#define STEPHI_A		(LATBCLR = BIT_7)
#define STEPLO_A		(LATBSET = BIT_7)
#define DIR_HI_A		(LATBCLR = BIT_6)
#define DIR_LO_A		(LATBSET = BIT_6)

#endif /* __HARDWARE_H__ */
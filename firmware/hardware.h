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

#define SYS_FREQ		(40000000ul)    /* 40 MHz */
#define GetSystemClock()	(SYS_FREQ)
#define	GetPeripheralClock()	(GetSystemClock())
#define	GetInstructionClock()	(GetSystemClock())

#define SPICHAN			2

/*    PORT USAGE
 *
 *	Pin	Port	Dir	Signal
 *
 *	2	RA0	OUT	Status LED
 *	9	RA2	OUT	STEP_Z
 *	10	RA3	OUT	DIR_X
 *	12	RA4	OUT	DIR_Z
 *	6	RB2	OUT	STEP_X
 *	7	RB3	OUT	STEP_Y
 *	11	RB4	OUT	DIR_Y
 *	14	RB5	OUT	MOTORS_ENABLE
 *	17	RB8	OUT	DATA READY
 *	22	RB11	OUT	MISO
 *	23	RB12	OUT	SPINDLE_ENABLE
 *	25	RB14	OUT	SPINDLE_PWM
 *
 *	3	RA1	IN	ABORT
 *	4	RB0	IN	HOLD
 *	5	RB1	IN	RESUME
 *	15	RB6	IN	LIM_X
 *	16	RB7	IN	LIM_Y
 *	18	RB9	IN	DATA REQUEST
 *	21	RB10	IN	LIM_Z
 *	24	RB13	IN	MOSI
 *	26	RB15	IN	SCLK 
 *
 */

#define LED_TOGGLE		(LATAINV = BIT_0)
#define REQ_IN			(PORTBbits.RB9)
#define RDY_LO			(LATBCLR = BIT_8)
#define RDY_HI			(LATBSET = BIT_8)

#define ABORT_IN		(PORTAbits.RA1)
#define HOLD_IN			(PORTBbits.RB0)
#define RESUME_IN		(PORTBbits.RB1)
#define LIM_X_IN		(PORTBbits.RB6)
#define LIM_Y_IN		(PORTBbits.RB7)
#define LIM_Z_IN		(PORTBbits.RB10)

#define MOTOR_EN_LO		(LATBCLR = BIT_5)
#define MOTOR_EN_HI		(LATBSET = BIT_5)
#define SPINDLE_EN_LO		(LATBCLR = BIT_12)
#define SPINDLE_EN_HI		(LATBSET = BIT_12)

#define STEP_X_LO		(LATBCLR = BIT_2)
#define STEP_X_HI		(LATBSET = BIT_2)
#define DIR_X_LO		(LATACLR = BIT_3)
#define DIR_X_HI		(LATASET = BIT_3)

#define STEP_Y_LO		(LATBCLR = BIT_3)
#define STEP_Y_HI		(LATBSET = BIT_3)
#define DIR_Y_LO		(LATBCLR = BIT_4)
#define DIR_Y_HI		(LATBSET = BIT_4)

#define STEP_Z_LO		(LATACLR = BIT_2)
#define STEP_Z_HI		(LATASET = BIT_2)
#define DIR_Z_LO		(LATACLR = BIT_4)
#define DIR_Z_HI		(LATASET = BIT_4)

#define STEP_A_LO		(LATBCLR = BIT_12)
#define STEP_A_HI		(LATBSET = BIT_12)
#define DIR_A_LO		(LATBCLR = BIT_14)
#define DIR_A_HI		(LATBSET = BIT_14)

#endif /* __HARDWARE_H__ */
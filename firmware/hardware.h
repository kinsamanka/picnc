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
 *	3	RA1	OUT	MISO
 *	9	RA2	OUT	STEP_Y
 *	10	RA3	OUT	DIR_Z
 *	12	RA4	OUT	DIR_Y
 *	4	RB0	OUT	DATA READY
 *	6	RB2	OUT	STEP_X
 *	7	RB3	OUT	DIR_X
 *	11	RB4	OUT	STEP_Z
 *	21	RB10	OUT	OUT3
 *	22	RB11	OUT	OUT0
 *	23	RB12	OUT	OUT1
 *	25	RB14	OUT	OUT2
 *
 *	5	RB1	IN	DATA REQUEST
 *	14	RB5	IN	INP0
 *	15	RB6	IN	INP4
 *	16	RB7	IN	INP3
 *	17	RB8	IN	INP2
 *	18	RB9	IN	INP1
 *	24	RB13	IN	MOSI
 *	26	RB15	IN	SCLK 
 *
 */

#define LED_TOGGLE		(LATAINV = BIT_0)
#define REQ_IN			(PORTBbits.RB1)
#define RDY_LO			(LATBCLR = BIT_0)
#define RDY_HI			(LATBSET = BIT_0)

#define INP0_IN			(PORTBbits.RB5)
#define INP1_IN			(PORTBbits.RB9)
#define INP2_IN			(PORTBbits.RB8)
#define INP3_IN			(PORTBbits.RB7)
#define INP4_IN			(PORTBbits.RB6)

#define OUT0_LO			(LATBCLR = BIT_11)
#define OUT0_HI			(LATBSET = BIT_11)
#define OUT1_LO			(LATBCLR = BIT_12)
#define OUT1_HI			(LATBSET = BIT_12)
#define OUT2_LO			(LATBCLR = BIT_14)
#define OUT2_HI			(LATBSET = BIT_14)
#define OUT3_LO			(LATBCLR = BIT_10)
#define OUT3_HI			(LATBSET = BIT_10)

#define STEP_X_LO		(LATBCLR = BIT_2)
#define STEP_X_HI		(LATBSET = BIT_2)
#define DIR_X_LO		(LATBCLR = BIT_3)
#define DIR_X_HI		(LATBSET = BIT_3)

#define STEP_Y_LO		(LATACLR = BIT_2)
#define STEP_Y_HI		(LATASET = BIT_2)
#define DIR_Y_LO		(LATACLR = BIT_4)
#define DIR_Y_HI		(LATASET = BIT_4)

#define STEP_Z_LO		(LATBCLR = BIT_4)
#define STEP_Z_HI		(LATBSET = BIT_4)
#define DIR_Z_LO		(LATACLR = BIT_3)
#define DIR_Z_HI		(LATASET = BIT_3)

#define STEP_A_LO		(LATBCLR = BIT_11)
#define STEP_A_HI		(LATBSET = BIT_11)
#define DIR_A_LO		(LATBCLR = BIT_12)
#define DIR_A_HI		(LATBSET = BIT_12)

#endif /* __HARDWARE_H__ */
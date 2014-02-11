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

#define SYS_FREQ		(80000000ul)    /* 80 MHz */
#define GetSystemClock()	(SYS_FREQ)
#define	GetPeripheralClock()	(GetSystemClock())
#define	GetInstructionClock()	(GetSystemClock())

#define SPICHAN			2

/*    PORT USAGE
 *
 *	Port	Dir	Signal
 *
 *	RE0	OUT	DIR_X
 *	RE1	OUT	STEP_X
 *	RE2	OUT	DIR_Y
 *	RE3	OUT	STEP_Y
 *	RE4	OUT	DIR_Z
 *	RE5	OUT	STEP_Z
 *	RE6	OUT	DIR_A
 *	RE7	OUT	STEP_A
 *	RG8	OUT	MISO
 *	RC13	OUT	Status LED
 *	RC14	OUT	DATA READY
 *
 *	RD0	OUT	PWM 0
 *	RD1	OUT	PWM 1
 *	RD2	OUT	PWM 2
 *
 *	RD3	OUT	OUTPUT 0
 *	RD4	OUT	OUTPUT 1
 *	RD5	OUT	OUTPUT 2
 *	RD6	OUT	OUTPUT 3
 *	RD7	OUT	OUTPUT 4
 *	RD8	OUT	OUTPUT 5
 *	RD9	OUT	OUTPUT 6
 *	RD10	OUT	OUTPUT 7
 *	RD11	OUT	OUTPUT 8
 *	RF0	OUT	OUTPUT 9
 *	RF1	OUT	OUTPUT 10
 *	RF3	OUT	OUTPUT 11
 *
 *	RG2	IN	DATA REQUEST
 *	RG3	IN	AUX
 *	RG6	IN	SCLK
 *	RG7	IN	MOSI
 *
 *	RB0	IN	ADC 0
 *	RB1	IN	ADC 1
 *	RB2	IN	ADC 2
 *
 *	RB3	IN	INPUT 0
 *	RB4	IN	INPUT 1
 *	RB5	IN	INPUT 2
 *	RB6	IN	INPUT 3
 *	RB7	IN	INPUT 4
 *	RB8	IN	INPUT 5
 *	RB9	IN	INPUT 6
 *	RB10	IN	INPUT 7
 *	RB11	IN	INPUT 8
 *	RB12	IN	INPUT 9
 *	RB13	IN	INPUT 10
 *	RB14	IN	INPUT 11
 *	RB15	IN	INPUT 12
 *
 */

#define LED_TOGGLE		(LATCINV = BIT_13)
#define REQ_IN			(PORTGbits.RG2)
#define RDY_LO			(LATCCLR = BIT_14)
#define RDY_HI			(LATCSET = BIT_14)

#define PORTD_OUT_MASK		(0xFF8)
#define PORTF_OUT_MASK		(BIT_0  | BIT_1  | BIT_3)

#define STEP_X_LO		(LATECLR = BIT_1)
#define STEP_X_HI		(LATESET = BIT_1)
#define DIR_X_LO		(LATECLR = BIT_0)
#define DIR_X_HI		(LATESET = BIT_0)

#define STEP_Y_LO		(LATECLR = BIT_3)
#define STEP_Y_HI		(LATESET = BIT_3)
#define DIR_Y_LO		(LATECLR = BIT_2)
#define DIR_Y_HI		(LATESET = BIT_2)

#define STEP_Z_LO		(LATECLR = BIT_5)
#define STEP_Z_HI		(LATESET = BIT_5)
#define DIR_Z_LO		(LATECLR = BIT_4)
#define DIR_Z_HI		(LATESET = BIT_4)

#define STEP_A_LO		(LATECLR = BIT_7)
#define STEP_A_HI		(LATESET = BIT_7)
#define DIR_A_LO		(LATECLR = BIT_6)
#define DIR_A_HI		(LATESET = BIT_6)

#endif /* __HARDWARE_H__ */
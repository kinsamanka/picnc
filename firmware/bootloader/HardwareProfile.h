
#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

#define __PIC32MX3XX_7XX__
#define SYS_FREQ		(80000000L)
#define GetSystemClock()	SYS_FREQ		// Hz
#define GetInstructionClock()	(GetSystemClock()/1)	// 
#define GetPeripheralClock()	(GetSystemClock()/1)	// Divisor is dependent on the value of FPBDIV set(configuration bits).

#define mLED			(LATCbits.LATC13)
#define BlinkLED()		(mLED = ((ReadCoreTimer() & 0x0100000) == 0))
#define InitLED()		do{TRISCbits.TRISC13 = 0; LATCbits.LATC13= 0;}while(0)
#define Error()			(LATCbits.LATC13= 1)

/* RX input */
#define ReadSwitchStatus()	(PORTReadBits(IOPORT_F, BIT_5) & BIT_5)

#endif  //HARDWARE_PROFILE_H

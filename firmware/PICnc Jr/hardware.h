#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#define SYS_FREQ		(40000000ul)    /* 40 kHz */
#define GetSystemClock()	(SYS_FREQ)
#define	GetPeripheralClock()	(GetSystemClock())
#define	GetInstructionClock()	(GetSystemClock())

#define SPICHAN			2

/* map SPI pins */

#define configure_PPS()								\
	do {									\
		PPSInput(3, SDI2, RPB2);					\
		PPSOutput(2, RPB1, SDO2);					\
	} while (0)

#define LED0_TRIS		(TRISBbits.TRISB5)
#define LED0_IO			(LATBbits.LATB5)

#define MOT_EN_TRIS		(TRISBbits.TRISB14)
#define MOT_EN_IO		(LATBbits.LATB14)

#define REQ_TRIS		(TRISAbits.TRISA1)
#define REQ_IO_IN		(PORTAbits.RA1)

#define RDY_TRIS		(TRISBbits.TRISB0)
#define RDY_IO			(LATBbits.LATB0)
#define RDY_IO_0		(LATBCLR = _LATB_LATB0_MASK)
#define RDY_IO_1		(LATBSET = _LATB_LATB0_MASK)

#define STEP_A_TRIS		(TRISBbits.TRISB7)
#define DIR_A_TRIS		(TRISBbits.TRISB6)
#define STEP_X_TRIS		(TRISBbits.TRISB13)
#define DIR_X_TRIS		(TRISBbits.TRISB12)
#define STEP_Y_TRIS		(TRISBbits.TRISB11)
#define DIR_Y_TRIS		(TRISBbits.TRISB10)
#define STEP_Z_TRIS		(TRISBbits.TRISB9)
#define DIR_Z_TRIS		(TRISBbits.TRISB8)

#define STEPHI_A		(LATBSET = _LATB_LATB7_MASK)
#define STEPLO_A		(LATBCLR = _LATB_LATB7_MASK)
#define DIR_HI_A		(LATBSET = _LATB_LATB6_MASK)
#define DIR_LO_A		(LATBCLR = _LATB_LATB6_MASK)

#define STEPHI_X		(LATBSET = _LATB_LATB13_MASK)
#define STEPLO_X		(LATBCLR = _LATB_LATB13_MASK)
#define DIR_HI_X		(LATBSET = _LATB_LATB12_MASK)
#define DIR_LO_X		(LATBCLR = _LATB_LATB12_MASK)

#define STEPHI_Y		(LATBSET = _LATB_LATB11_MASK)
#define STEPLO_Y		(LATBCLR = _LATB_LATB11_MASK)
#define DIR_HI_Y		(LATBSET = _LATB_LATB10_MASK)
#define DIR_LO_Y		(LATBCLR = _LATB_LATB10_MASK)

#define STEPHI_Z		(LATBSET = _LATB_LATB9_MASK)
#define STEPLO_Z		(LATBCLR = _LATB_LATB9_MASK)
#define DIR_HI_Z		(LATBSET = _LATB_LATB8_MASK)
#define DIR_LO_Z		(LATBCLR = _LATB_LATB8_MASK)

#endif /* __HARDWARE_H__ */
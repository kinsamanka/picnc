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
		PPSInput(3, SDI2, RPB13);					\
		PPSOutput(2, RPA1, SDO2);					\
	} while (0)

#define LED0_TRIS		(TRISAbits.TRISA0)
#define LED0_IO			(LATAbits.LATA0)

#define REQ_TRIS		(TRISBbits.TRISB1)
#define REQ_IO_IN		(PORTBbits.RB1)

#define RDY_TRIS		(TRISBbits.TRISB0)
#define RDY_IO			(LATBbits.LATB0)
#define RDY_IO_0		(LATBCLR = _LATB_LATB0_MASK)
#define RDY_IO_1		(LATBSET = _LATB_LATB0_MASK)

#define STEP_A_TRIS		(TRISBbits.TRISB12)
#define DIR_A_TRIS		(TRISBbits.TRISB14)
#define STEP_X_TRIS		(TRISBbits.TRISB2)
#define DIR_X_TRIS		(TRISBbits.TRISB3)
#define STEP_Y_TRIS		(TRISBbits.TRISB4)
#define DIR_Y_TRIS		(TRISAbits.TRISA4)
#define STEP_Z_TRIS		(TRISAbits.TRISA2)
#define DIR_Z_TRIS		(TRISAbits.TRISA3)

/* Note the outputs are inverted */

#define STEPHI_A		(LATBCLR = _LATB_LATB12_MASK)
#define STEPLO_A		(LATBSET = _LATB_LATB12_MASK)
#define DIR_HI_A		(LATBCLR = _LATB_LATB14_MASK)
#define DIR_LO_A		(LATBSET = _LATB_LATB14_MASK)

#define STEPHI_X		(LATBCLR = _LATB_LATB2_MASK)
#define STEPLO_X		(LATBSET = _LATB_LATB2_MASK)
#define DIR_HI_X		(LATBCLR = _LATB_LATB3_MASK)
#define DIR_LO_X		(LATBSET = _LATB_LATB3_MASK)

#define STEPHI_Y		(LATBCLR = _LATB_LATB4_MASK)
#define STEPLO_Y		(LATBSET = _LATB_LATB4_MASK)
#define DIR_HI_Y		(LATACLR = _LATA_LATA4_MASK)
#define DIR_LO_Y		(LATASET = _LATA_LATA4_MASK)

#define STEPHI_Z		(LATACLR = _LATA_LATA2_MASK)
#define STEPLO_Z		(LATASET = _LATA_LATA2_MASK)
#define DIR_HI_Z		(LATACLR = _LATA_LATA3_MASK)
#define DIR_LO_Z		(LATASET = _LATA_LATA3_MASK)

#endif /* __HARDWARE_H__ */
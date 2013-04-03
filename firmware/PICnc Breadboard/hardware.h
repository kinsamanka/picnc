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

#define configure_inputs()							\
	do {									\
		TRISBSET = BIT_9 | BIT_8 | BIT_7 | BIT_6 | BIT_5 ;		\
	} while (0)

#define LED0_TRIS		(TRISAbits.TRISA0)
#define LED0_IO			(LATAbits.LATA0)

#define REQ_TRIS		(TRISBbits.TRISB1)
#define REQ_IO_IN		(PORTBbits.RB1)

#define REQ_CNPU_Enable()							\
	do {									\
		ConfigCNBPullups(CNB1_PULLUP_ENABLE);				\
	} while (0)

#define RDY_TRIS		(TRISBbits.TRISB0)
#define RDY_IO			(LATBbits.LATB0)
#define RDY_IO_0		(LATBCLR = _LATB_LATB0_MASK)
#define RDY_IO_1		(LATBSET = _LATB_LATB0_MASK)

#define configure_stepdir()							\
	do {									\
		TRISACLR = BIT_4 | BIT_3 | BIT_2;				\
		TRISBCLR = BIT_14 | BIT_12 | BIT_4 | BIT_3 | BIT_2;		\
	} while (0)

/* Note the outputs are inverted */

#define STEPHI_A		(LATBCLR = _LATB_LATB12_MASK)
#define STEPLO_A		(LATBSET = _LATB_LATB12_MASK)
#define DIR_HI_A		(LATBCLR = _LATB_LATB14_MASK)
#define DIR_LO_A		(LATBSET = _LATB_LATB14_MASK)

#define STEPHI_X		(LATACLR = _LATA_LATA3_MASK)
#define STEPLO_X		(LATASET = _LATA_LATA3_MASK)
#define DIR_HI_X		(LATACLR = _LATA_LATA2_MASK)
#define DIR_LO_X		(LATASET = _LATA_LATA2_MASK)

#define STEPHI_Y		(LATBCLR = _LATB_LATB2_MASK)
#define STEPLO_Y		(LATBSET = _LATB_LATB2_MASK)
#define DIR_HI_Y		(LATBCLR = _LATB_LATB3_MASK)
#define DIR_LO_Y		(LATBSET = _LATB_LATB3_MASK)

#define STEPHI_Z		(LATACLR = _LATA_LATA4_MASK)
#define STEPLO_Z		(LATASET = _LATA_LATA4_MASK)
#define DIR_HI_Z		(LATBCLR = _LATB_LATB4_MASK)
#define DIR_LO_Z		(LATBSET = _LATB_LATB4_MASK)



#endif /* __HARDWARE_H__ */
#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#define SYS_FREQ		(80000000ul)    // Hz
#define GetSystemClock()	(SYS_FREQ)
#define	GetPeripheralClock()	(GetSystemClock())
#define	GetInstructionClock()	(GetSystemClock())

#define SPICHAN			2

#define LED0_TRIS		TRISCbits.TRISC13
#define LED0_IO			LATCbits.LATC13

#define REQ_TRIS		TRISGbits.TRISG2
#define REQ_IO_IN		PORTGbits.RG2

#define AUX_TRIS		TRISGbits.TRISG3
#define AUX_IO_IN		PORTGbits.RG3

#define RDY_TRIS		TRISCbits.TRISC14
#define RDY_IO			LATCbits.LATC14
#define RDY_IO_0		(LATCCLR = _LATC_LATC14_MASK)
#define RDY_IO_1		(LATCSET = _LATC_LATC14_MASK)

#define STEPHI_A		(LATESET = _LATE_LATE7_MASK)
#define STEPLO_A		(LATECLR = _LATE_LATE7_MASK)
#define DIR_HI_A		(LATESET = _LATE_LATE6_MASK)
#define DIR_LO_A		(LATECLR = _LATE_LATE6_MASK)

#define STEPHI_X		(LATESET = _LATE_LATE1_MASK)
#define STEPLO_X		(LATECLR = _LATE_LATE1_MASK)
#define DIR_HI_X		(LATESET = _LATE_LATE0_MASK)
#define DIR_LO_X		(LATECLR = _LATE_LATE0_MASK)

#define STEPHI_Y		(LATESET = _LATE_LATE3_MASK)
#define STEPLO_Y		(LATECLR = _LATE_LATE3_MASK)
#define DIR_HI_Y		(LATESET = _LATE_LATE2_MASK)
#define DIR_LO_Y		(LATECLR = _LATE_LATE2_MASK)

#define STEPHI_Z		(LATESET = _LATE_LATE5_MASK)
#define STEPLO_Z		(LATECLR = _LATE_LATE5_MASK)
#define DIR_HI_Z		(LATESET = _LATE_LATE4_MASK)
#define DIR_LO_Z		(LATECLR = _LATE_LATE4_MASK)

#endif /* __HARDWARE_H__ */
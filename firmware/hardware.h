#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#define SYS_FREQ		(80000000ul)    // Hz
#define GetSystemClock()	(SYS_FREQ)
#define	GetPeripheralClock()	(GetSystemClock())
#define	GetInstructionClock()	(GetSystemClock())

#define SPICHAN			2

#define LED0_TRIS		TRISFbits.TRISF3
#define LED0_IO			LATFbits.LATF3

#define REQ_TRIS		TRISDbits.TRISD2
#define REQ_IO			PORTDbits.RD2

#define RDY_TRIS		TRISDbits.TRISD1
#define RDY_IO			LATDbits.LATD1
#define RDY_IO_0		(LATDCLR = _LATD_LATD1_MASK)
#define RDY_IO_1		(LATDSET = _LATD_LATD1_MASK)

#define STEPHI_A		(LATESET = _LATE_LATE0_MASK)
#define STEPLO_A		(LATECLR = _LATE_LATE0_MASK)
#define DIR_HI_A		(LATESET = _LATE_LATE1_MASK)
#define DIR_LO_A		(LATECLR = _LATE_LATE1_MASK)

#define STEPHI_X		(LATESET = _LATE_LATE4_MASK)
#define STEPLO_X		(LATECLR = _LATE_LATE4_MASK)
#define DIR_HI_X		(LATESET = _LATE_LATE7_MASK)
#define DIR_LO_X		(LATECLR = _LATE_LATE7_MASK)

#define STEPHI_Y		(LATESET = _LATE_LATE6_MASK)
#define STEPLO_Y		(LATECLR = _LATE_LATE6_MASK)
#define DIR_HI_Y		(LATESET = _LATE_LATE5_MASK)
#define DIR_LO_Y		(LATECLR = _LATE_LATE5_MASK)

#define STEPHI_Z		(LATESET = _LATE_LATE2_MASK)
#define STEPLO_Z		(LATECLR = _LATE_LATE2_MASK)
#define DIR_HI_Z		(LATESET = _LATE_LATE3_MASK)
#define DIR_LO_Z		(LATECLR = _LATE_LATE3_MASK)

#endif /* __HARDWARE_H__ */
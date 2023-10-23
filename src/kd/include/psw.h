#ifndef _PSW_H_
#define _PSW_H_

// The following macro's define the format of the Processor Status Word (PSW).
// 
// The PSW in the LSI-11 is a composite of (1) the 4 PDP-11 condition codes
// (N,Z,V,C) [bits 0:3], (2) the Trace Bit [bit 4] and (3) the Interrupt Enable
// bit [bit 7]. (EK-KUV11-TM-001 pg. 1-9). 
// 
// | 15 | 14 | 13 | 12 | 11 | 10 | 09 | 08 | 07 | 06 | 05 | 04 | 03 | 02 | 01 | 00 |
// |                                       |   Priority   |  T |  N |  Z |  V |  C | 
// 
// T - When set, a processor trap will occur through location 14 at completion
//     of the current instruction execution, and a new processor status word will
//     be loaded from location 16.
// Z = 1, if the result was zero 
// N = 1, if the result was negative 
// C = l, if the operation resulted in a carry from the MSB (most significant bit) or
//        a 1 was shifted from MSB or LSB (least significant bit) 
// V = 1, if the operation resulted in an arithmetic overflow
// 
// Processor Status Word bits 5 and 6 are reserved (EK-KUV11-TM-001
// Figure 2-5). These bits can be set and cleared by the MTPS instruction
// however. At least every FIS instruction resets bits 5 and 6. This
// behaviour is confirmed by the VKAAC0 (Basic Intruction Tests) and VKACC0
// diagnostics.
//
// The KDF11-A PSW format:
//
// | 15 | 14 | 13 | 12 | 11 | 10 | 09 | 08 | 07 | 06 | 05 | 04 | 03 | 02 | 01 | 00 |
// |   CM    |    PM   |  Reserved    | SI |   Priority   |  T |  N |  Z |  V |  C | 
//
// CM - Current Memory Management Mode
// PM - Previous Memory Management Mode
// SI - Suspended Instruction
// Priority - Priority Level
// T-bit and conditions codes as described above
//
#define	PSW_C				        _BV(0)
#define	PSW_V				        _BV(1)
#define	PSW_Z				        _BV(2)
#define	PSW_N				        _BV(3)
#define	PSW_T				        _BV(4)
#define	PSW_PRIO			        _BV(7)
#define	PSW_PRIORITY		        (_BV(7) | _BV(6) | _BV(5))
#define	PSW_MEM_MGMT_MODE_P	        14
#define	PSW_MEM_MGMT_MODE	        (_BV(15) | _BV(14))
#define PSW_PREV_MEM_MGMT_MODE_P    12
#define	PSW_PREV_MEM_MGMT_MODE      (_BV(13) | _BV(12))

#endif // _PSW_H_
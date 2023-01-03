#ifndef __TRACE_H__
#define __TRACE_H__

#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#define	MAGIC_CPU0		0x43505530
#define	MAGIC_CPUZ		0x4350555a
#define	MAGIC_CPU1		0x43505531
#define MAGIC_DURA		0x44555241		// Duration
#define	MAGIC_BUS0		0x42555330
#define	MAGIC_BUS1		0x42555331
#define	MAGIC_TRAP		0x54524150
#define	MAGIC_IRQ0		0x49525130
#define	MAGIC_RX2A		0x52583241
#define	MAGIC_RX2C		0x52583243
#define	MAGIC_RX2D		0x52583244
#define	MAGIC_RX2E		0x52583245
#define	MAGIC_RX2S		0x52583253
#define	MAGIC_DLV1		0x444C5631
#define MAGIC_RL2A		0x524C3241		// RLV12 register
#define MAGIC_RL2C		0x524C3243		// RLV12 command

#define	TRC_CPU_TRAP		0
#define	TRC_CPU_HALT		1
#define	TRC_CPU_WAIT		2
#define	TRC_CPU_RUN			3
#define	TRC_CPU_DBLBUS		4
#define	TRC_CPU_ODT_P		5
#define	TRC_CPU_ODT_G		5

#define	TRC_TRAP			0
#define	TRC_TRAP_ILL		1
#define	TRC_TRAP_RADDR		2
#define	TRC_TRAP_T			3

#define	TRC_BUS_RD			0
#define	TRC_BUS_WR			1
#define	TRC_BUS_RDFAIL		2
#define	TRC_BUS_WRFAIL		3
#define	TRC_BUS_RESET		4

#define	TRC_IRQ_OK			0
#define	TRC_IRQ_FAIL		1
#define	TRC_IRQ_SIG			2

#define	TRC_DLV11_RX		0
#define	TRC_DLV11_TX		1
#define	TRC_DLV11_RDY		2
#define	TRC_DLV11_SEI		3
#define	TRC_DLV11_CLI		4

#define	TRC_RXV21_FILL		0
#define	TRC_RXV21_EMPTY		1
#define	TRC_RXV21_WRITE		2
#define	TRC_RXV21_READ		3
#define	TRC_RXV21_SET_MD	4
#define	TRC_RXV21_STATUS	5
#define	TRC_RXV21_WRITE_DD	6
#define	TRC_RXV21_READ_ERR	7

#define	TRC_RXV21_WC_OVFL	0
#define	TRC_RXV21_DEN_ERR	1
#define	TRC_RXV21_TRACK_NO	2
#define	TRC_RXV21_SECT_NO	3

// Definition of trace flags. There are two sets of flags:
// - Flags definining the way the trace is to be output
//   (TRACEF_WRITE, TRACEF_PRINT, TRACE_COMPRESS and TRACEF_FIRST_Z).
// - Flags defining the trace category. By defining the separate
//   flags the trace output can be tuned.
// 
// TRACEF_IGNORE_BUS is a special flag; it temporarily overrides the
// TRACEF_BUS, TRACEF_MEMORYDUMP and TRACEF_IRQ flags.
//
// ToDo: Transform TRACEF flags to Bitmasks.
// ToDo: Change category flags to constexpr's so the compiler can remove
// the calls.
//
#define	TRACEF_WRITE			(1 << 0)
#define	TRACEF_IGNORE_BUS		(1 << 1)
#define	TRACEF_PRINT			(1 << 2)
#define	TRACEF_COMPRESS			(1 << 3)
#define	TRACEF_FIRST_Z			(1 << 4)
#define	TRACEF_STEP				(1 << 5)
#define TRACEF_CPUEVENT			(1 << 6)
#define TRACEF_BUS				(1 << 7)
#define TRACEF_MEMORYDUMP		(1 << 8)
#define TRACEF_TRAP				(1 << 9)
#define TRACEF_IRQ				(1 << 10)
#define TRACEF_DLV11			(1 << 11)
#define TRACEF_RXV21CMD			(1 << 12)
#define TRACEF_RXV21STEP		(1 << 13)
#define TRACEF_RXV21DMA			(1 << 14)
#define TRACEF_RXV21ERROR		(1 << 15)
#define TRACEF_RXV21DISK		(1 << 16)
#define TRACEF_RLV12			(1 << 17)
#define TRACEF_DURATION			(1 << 18)

#define	TRCSETIGNBUS()	(trc.flags |= TRACEF_IGNORE_BUS)
#define	TRCCLRIGNBUS()	(trc.flags &= ~TRACEF_IGNORE_BUS)
#define	IFTRC()		if(trc.flags)
#define	__IFTRC(x)	{ IFTRC() x; }
#define	TRCStep(r, psw, insn) \
		__IFTRC(trc.TRACEStep(r, psw, insn))
#define	TRCCPUEvent(event, info) \
		__IFTRC(trc.TRACECPUEvent(event, info))
#define	TRCBus(type, addr, val) \
		__IFTRC(trc.TRACEBus(type, addr, val))
#define	TRCMemoryDump(ptr, addr, val) \
		__IFTRC(trc.TRACEMemoryDump(ptr, addr, len))
#define	TRCIRQ(n, type) \
		__IFTRC(trc.TRACEIrq(n, type))
#define	TRCTrap(n, cause) \
		__IFTRC(trc.TRACETrap(n, cause))
#define	TRCDLV11(type, channel, value) \
		__IFTRC(trc.TRACEDLV11(channel, type, value))
#define	TRCRXV21CMD(type, rx2cs) \
		__IFTRC(trc.TRACERXV21Command(0, type, rx2cs))
#define	TRCRXV21CMDCommit(type, rx2cs) \
		__IFTRC(trc.TRACERXV21Command(1, type, rx2cs))
#define	TRCRXV21Step(type, step, rx2db) \
		__IFTRC(trc.TRACERXV21Step(type, step, rx2db))
#define	TRCRXV21DMA(type, rx2wc, rx2ba) \
		__IFTRC(trc.TRACERXV21DMA(type, rx2wc, rx2ba))
#define	TRCRXV21Disk(type, drive, density, rx2sa, rx2ta) \
		__IFTRC(trc.TRACERXV21Disk(type, drive, density, rx2sa, rx2ta))
#define	TRCRXV21Error(type, info) \
		__IFTRC(trc.TRACERXV21Error(type, info))

#define	TRCINIT(name)	trc.TRACEOpen(name);
#define	TRCFINISH()	trc.TRACEClose();

struct TRACE_CPU
{
	u32	magic;
	u16	r[8];
	u16	psw;
	u16	insn[3];
	u32	pad;
	u64	step;
};

struct TRACE_CPUZ
{
	u32	magic;
	u16	pc;
	u16	mask;
	u64	step;
	u16	data[11];
};

struct TRACE_CPUZS
{
	u32	magic;
	u16	pc;
	u16	mask;
	u32	step;
	u16	data[11];
};

struct TRACE_CPUEVT
{
	u32	magic;
	u16	type;
	u16	value;
};

struct TRACE_BUS
{
	u32	magic;
	u16	addr;
	u16	value;
	u16	type;
	u16	pad;
};

struct TRACE_MEMDUMP
{
	u32	magic;
	u16	addr;
	u16	len;
};

struct TRACE_TRAP
{
	u32	magic;
	u16	trap;
	u16	cause;
};

struct TRACE_IRQ
{
	u32	magic;
	u16	trap;
	u16	type;
};

struct TRACE_RX02
{
	u32	magic;
	u16	rx2cs;
	u16	rx2ta;
	u16	rx2sa;
	u16	rx2wc;
	u16	rx2ba;
	u16	rx2es;
	u16	command;
	u16	status;
};

struct TRACE_DLV11
{
	u32	magic;
	u8	channel;
	u8	type;
	u16	value;
};

struct TRACE_RXV21CMD
{
	u32	magic;
	u8	type;
	u8	commit;
	u16	rx2cs;
};

struct TRACE_RXV21STEP
{
	u32	magic;
	u8	type;
	u8	step;
	u16	rx2db;
};

struct TRACE_RXV21DMA
{
	u32	magic;
	u16	type;
	u16	rx2wc;
	u16	rx2ba;
	u16	pad;
};

struct TRACE_RXV21DISK
{
	u32	magic;
	u16	type;
	u8	drive;
	u8	density;
	u16	rx2sa;
	u16	rx2ta;
};

struct TRACE_RXV21ERR
{
	u32	magic;
	u16	type;
	u16	info;
};

struct TRACE_RLV12REGS
{
	u32 magic;
	u16 rlcs;
    u16 rlba;
    u16 rlda;
    u16 rlmpr;
    u16 rlbae;
	u16 length;
};


struct TRACE_RLV12COMMAND
{
	u32 magic;
	u16 command;
};

struct TRACE_DURATION
{
	u32 magic;
	u32 durationCount;		// in microseconds
	u16 length;				// length of remaing chars
};

class TRACE
{
private:

	void limitFileSize ();
	const char* rxv21_get_cmd_name(int func);
	const char* rxv21_get_error_name(int type);
	const char* rlv12GetCommandName (u16 command);

public:
	std::string baseFileName {};
	FILE*	file {};
	u64	step {};
	int	flags {TRACEF_WRITE | TRACEF_FIRST_Z};
	u16	last_psw {};
	u16	last_r[7] {};

	// TRACE ();
	int	TRACEOpen (const char* filename);
	void TRACEClose ();
	void TRACEStep (u16* r, u16 psw, u16* insn);
	void TRACECPUEvent (int type, u16 value);
	void TRACEBus (u16 type, u16 address, u16 value);
	void TRACEMemoryDump (u8* ptr, u16 address, u16 length);
	void TRACETrap (int n, int cause);
	void TRACEIrq (int n, int type);
	void TRACEDLV11 (int channel, int type, u16 value);
	void TRACERXV21Command (int commit, int type, u16 rx2cs);
	void TRACERXV21Step (int type, int step, u16 rx2db);
	void TRACERXV21DMA (int type, u16 rx2wc, u16 rx2ba);
	void TRACERXV21Disk (int type, int drive, int density,
			u16 rx2sa, u16 rx2ta);
	void TRACERXV21Error (int type, u16 info);
	void TRACERLV12Registers (const char *msg, u16 rlcs, 
			u16 rlba, u16 rlda, u16 rlmpr, u16 rlbae);
	void TRACERLV12Command (u16 command);
	void TRACEDuration (const char *msg, u32 duration);
};

extern TRACE trc;

// Constructor for the TRACE struct
/*
TRACE::TRACE ()
	:
	baseFileName {},
	file {0},
	step {0},
	flags {0},
	last_psw {0},
	last_r {0}
{}
*/

#endif

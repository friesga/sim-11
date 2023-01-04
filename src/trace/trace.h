#ifndef __TRACE_H__
#define __TRACE_H__

#include "types.h"
#include "kd11/cpu/kd11cpu.h"   // For PSW_* macro's

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <mutex>

using std::mutex;
using std::lock_guard;

#define	DST	stderr

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

#define	TRCSETIGNBUS()	(trc.flags |= TRACE::IgnoreBus)
#define	TRCCLRIGNBUS()	(trc.flags &= ~TRACE::IgnoreBus)

// Setting traceEnabled to false will optimize out all calls to trace
// functions.
static constexpr bool traceEnabled = true;

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

/* LSI-11 disassembler */
extern int  LSI11Disassemble (const u16* insn, u16 pc, char* buf);
extern int  LSI11InstructionLength (const u16* insn);

class TRACE
{
private:
    std::string baseFileName {};
    u64	step {};
    u16	last_psw {};
	u16	last_r[7] {};

    mutex traceFileMutex;
	void limitFileSize ();
	const char* rxv21_get_cmd_name(int func);
	const char* rxv21_get_error_name(int type);
	const char* rlv12GetCommandName (u16 command);
    const char* get_trap_name (int n);

public:
    // Definition of trace flags. There are two sets of flags:
    // - Flags definining the way the trace is to be output
    //   (Write, Print, Compress and FirstZ).
    // - Flags defining the trace category. By defining the separate
    //   flags the trace output can be tuned.
    // 
    // IgnoreBus is a special flag; it temporarily overrides the
    // Bus, MemoryDump and Irq flags.
    //
    enum Category
    {
        Write      = (1 << 0),
        IgnoreBus  = (1 << 1),
        Print	   = (1 << 2),
        Compress   = (1 << 3),
        FirstZ     = (1 << 4),
        Step	   = (1 << 5),
        CpuEvent   = (1 << 6),
        Bus		   = (1 << 7),
        MemoryDump = (1 << 8),
        Trap	   = (1 << 9),
        Irq		   = (1 << 10),
        Dlv11	   = (1 << 11),
        RXV21Cmd   = (1 << 12),
        RXV21Step  = (1 << 13),
        RXV21Dma   = (1 << 14),
        RXV21Error = (1 << 15),
        RXV21Disk  = (1 << 16),
        RLV12	   = (1 << 17),
        Duration   = (1 << 18)
    };

	FILE* file {};
	int flags {Write | FirstZ};

	// Definition of the public member functions as templates with a boolean
	// non-type member with default value false and a default empty body. 
	// When called without template parameter or with the parameter false,
	// calls to these functions will be optimized out by the compiler. Only
	// calls withe the parameter set to true will result in a function call.
	template <bool traceEnabled = false>
		void TRACEOpen (const char* filename) {}
	template <bool traceEnabled = false>
		void TRACEClose () {}
	template <bool traceEnabled = false>
		void TRACEStep (u16* r, u16 psw, u16* insn) {}
	template <bool traceEnabled = false>
		void TRACECPUEvent (int type, u16 value) {}
	template <bool traceEnabled = false>
		void TRACEBus (u16 type, u16 address, u16 value) {}
	template <bool traceEnabled = false>
		void TRACEMemoryDump (u8* ptr, u16 address, u16 length) {}
	template <bool traceEnabled = false>
		void TRACETrap (int n, int cause) {}
	template <bool traceEnabled = false>
		void TRACEIrq (int n, int type) {}
	template <bool traceEnabled = false>
		void TRACEDLV11 (int channel, int type, u16 value) {}
	template <bool traceEnabled = false>
		void TRACERXV21Command (int commit, int type, u16 rx2cs) {}
	template <bool traceEnabled = false>
		void TRACERXV21Step (int type, int step, u16 rx2db) {}
	template <bool traceEnabled = false>
		void TRACERXV21DMA (int type, u16 rx2wc, u16 rx2ba) {}
	template <bool traceEnabled = false>
		void TRACERXV21Disk (int type, int drive, int density,
		u16 rx2sa, u16 rx2ta) {}
	template <bool traceEnabled = false>
		void TRACERXV21Error (int type, u16 info) {}
	template <bool traceEnabled = false>
		void TRACERLV12Registers (const char *msg, u16 rlcs, 
		u16 rlba, u16 rlda, u16 rlmpr, u16 rlbae) {}
	template <bool traceEnabled = false>
		void TRACERLV12Command (u16 command) {}
	template <bool traceEnabled = false>
		void TRACEDuration (const char *msg, u32 duration) {}
};

extern TRACE trc;

// Convenience functions. Calls to these functions are optimized out by
// the compiler if traceEnabled is set to false.
extern void TRCOpen (const char* f);
extern void TRCClose();
extern void TRCStep(u16* r, u16 psw, u16* insn);
extern void TRCCPUEvent(int type, u16 value);
extern void TRCBus(u16 type, u16 address, u16 value);
extern void TRCMemoryDump(u8* ptr, u16 address, u16 length);
extern void TRCIRQ(int n, int type);
extern void TRCTrap(int n, int cause);
extern void TRCDLV11(int channel, int type, u16 value);
extern void TRCRXV21CMD(int type, u16 rx2cs);
extern void TRCRXV21CMDCommit(int type, u16 rx2cs);
extern void TRCRXV21Step(int type, int step, u16 rx2db);
extern void TRCRXV21DMA(int type, u16 rx2wc, u16 rx2ba);
extern void TRCRXV21Disk(int type, int drive, int density, u16 rx2sa, u16 rx2ta);
extern void TRCRXV21Error(int type, u16 info);

template <> 
inline void TRACE::TRACEOpen<true> (const char* filename)
{
    char header[6] = { 'X', 'T', 'R', 'C', 0, 65 };

    file = fopen (filename, "wb");
    if (!file)
        throw "Error: cannot open tracefile " + std::string(filename);

    if (baseFileName.empty ())
        baseFileName = std::string (filename);
    step = 0;
    memset (last_r, 0, 7 * sizeof (u16));
    last_psw = 0;
    fwrite (&header, 6, 1, file);
}

template <> 
inline void TRACE::TRACEClose<true> ()
{
    fclose (file);
}

template <> 
inline void TRACE::TRACEStep<true> (u16* r, u16 psw, u16* insn)
{
    int i;
    char buf[64];

    if (!(flags & Step))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        LSI11Disassemble (insn, r[7], buf);
        fprintf (DST, "PC=%06o PSW [%c%c%c%c%c%c] SP=%06o [R0=%06o R1=%06o R2=%06o R3=%06o R4=%06o R5=%06o] %06o => %s\n",
            r[7],
            PSW_GET (PSW_PRIO) ? 'P' : '-',
            PSW_GET (PSW_T) ? 'T' : '-',
            PSW_GET (PSW_N) ? 'N' : '-',
            PSW_GET (PSW_Z) ? 'Z' : '-',
            PSW_GET (PSW_V) ? 'V' : '-',
            PSW_GET (PSW_C) ? 'C' : '-',
            r[6], r[0], r[1], r[2], r[3], r[4], r[5],
            *insn, buf);
    }

    if (flags & Write)
    {
        if ((flags & Compress) && !(flags & FirstZ))
        {
            int len = LSI11InstructionLength (insn);
            int cnt = len;
            int off = 0;
            u16 mask = 0;
            for (i = 0; i < 7; i++)
            {
                if (last_r[i] != r[i])
                {
                    mask |= (1 << i);
                    cnt++;
                }
            }
            if (last_psw != psw)
            {
                mask |= 0x80;
                cnt++;
            }

            if (step < 0xFFFFFFFFL)
            {
                TRACE_CPUZS rec;
                rec.magic = U32B (MAGIC_CPUZ);
                rec.step = U32B ((u32)step++);
                rec.pc = U16B (r[7]);
                rec.mask = U16B ((len << 8) | mask);
                for (i = 0; i < len; i++)
                    rec.data[off++] = U16B (insn[i]);

                for (i = 0; i < 7; i++)
                {
                    if (last_r[i] != r[i])
                    {
                        rec.data[off++] = U16B (r[i]);
                        last_r[i] = r[i];
                    }
                }
                if (last_psw != psw)
                {
                    rec.data[off++] = U16B (psw);
                    last_psw = psw;
                }
                fwrite (&rec, 12 + cnt * 2, 1, file);
            }
            else
            {
                TRACE_CPUZ rec;
                rec.magic = U32B (MAGIC_CPUZ);
                rec.step = U64B (step++);
                rec.pc = U16B (r[7]);
                rec.mask = U16B (0x8000 | (len << 8) | mask);
                for (i = 0; i < len; i++)
                    rec.data[off++] = U16B (insn[i]);

                for (i = 0; i < 7; i++)
                {
                    if (last_r[i] != r[i])
                    {
                        rec.data[off++] = U16B (r[i]);
                        last_r[i] = r[i];
                    }
                }
                if (last_psw != psw)
                {
                    rec.data[off++] = U16B (psw);
                    last_psw = psw;
                }
                fwrite (&rec, 16 + cnt * 2, 1, file);
            }
        }
        else
        {
            TRACE_CPU rec;
            flags &= ~FirstZ;
            rec.magic = U32B (MAGIC_CPU0);
            rec.psw = U16B (psw);
            rec.step = U64B (step++);
            memcpy (&rec.insn, insn, 6);
            memcpy (&rec.r, r, 16);
            for (i = 0; i < 3; i++)
            {
                rec.insn[i] = U16B (rec.insn[i]);
            }
            for (i = 0; i < 8; i++)
            {
                rec.r[i] = U16B (rec.r[i]);
            }
            fwrite (&rec, sizeof (rec), 1, file);

            // Don't flush instruction trace records as flushing slows tracing down
            // to an unacceptable level
            // fflush (file);
            limitFileSize ();
        }
    }
}

template <> 
inline void TRACE::TRACECPUEvent<true> (int type, u16 value)
{
    TRACE_CPUEVT rec;

    if (!(flags & CpuEvent))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        switch (type)
        {
            case TRC_CPU_TRAP:
                fprintf (DST, "[KD11] handling trap: %o\n", value);
                break;
            case TRC_CPU_HALT:
                fprintf (DST, "[KD11] HALT @ %06o\n", value);
                break;
            case TRC_CPU_WAIT:
                fprintf (DST, "[KD11] HALT @ %06o\n", value);
                break;
            case TRC_CPU_RUN:
                fprintf (DST, "[KD11] resume execution at %06o\n", value);
                break;
            case TRC_CPU_DBLBUS:
                fprintf (DST, "[KD11] double bus error [%06o]\n", value);
                break;
        }
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_CPU1);
        rec.type = U16B (type);
        rec.value = U16B (value);
        fwrite (&rec, sizeof (rec), 1, file);
        fflush (file);
        limitFileSize ();
    }
}

template <> 
inline void TRACE::TRACEBus<true> (u16 type, u16 address, u16 value)
{
    TRACE_BUS rec;

    if (!(flags & Bus) || flags & IgnoreBus)
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        switch (type)
        {
            case TRC_BUS_RD:
                fprintf (DST, "[QBUS] read  %06o = %06o\n", address, value);
                break;
            case TRC_BUS_RDFAIL:
                fprintf (DST, "[QBUS] read %06o timed out\n", address);
                break;
            case TRC_BUS_WR:
                fprintf (DST, "[QBUS] write %06o = %06o\n", address, value);
                break;
            case TRC_BUS_WRFAIL:
                fprintf (DST, "[QBUS] write %06o = %06o timed out\n", address, value);
                break;
            case TRC_BUS_RESET:
                fprintf (DST, "[QBUS] reset\n");
                break;
        }
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_BUS0);
        rec.type = U16B (type);
        rec.addr = U16B (address);
        rec.value = U16B (value);
        fwrite (&rec, sizeof (rec), 1, file);
        fflush (file);
        limitFileSize ();
    }
}

template <> 
inline void TRACE::TRACEMemoryDump<true> (u8* ptr, u16 address, u16 length)
{
    TRACE_MEMDUMP rec;

    if (!(flags & MemoryDump) || flags & IgnoreBus)
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        int i = 0;
        fprintf (DST, "[MEM] dump %d from %06o:\n", length, address);
        for (i = 0; i < length; i++)
        {
            if ((i % 8) == 7)
                fprintf (DST, "\n");
            
            fprintf (DST, " %03o", ptr[i]);
        }
        fprintf (DST, "\n");
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_BUS1);
        rec.addr = U16B (address);
        rec.len = U16B (length);
        fwrite (&rec, sizeof (rec), 1, file);
        fwrite (ptr, length, 1, file);
        fflush (file);
        limitFileSize ();
    }
}


template <> 
inline void TRACE::TRACETrap<true> (int n, int cause)
{
    TRACE_TRAP rec;

    if (!(flags & Trap))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        const char* name;
        switch (cause)
        {
            case TRC_TRAP:
                name = get_trap_name (n);
                if (name)
                    fprintf (DST, "[KD11] TRAP %o: %s\n", n, name);
                else
                    fprintf (DST, "[KD11] TRAP %o\n", n);
                break;

            case TRC_TRAP_T:
                fprintf (DST, "[KD11] TRAP %o: T bit\n", n);
                break;

            case TRC_TRAP_RADDR:
                fprintf (DST, "[KD11] TRAP %o: get address on mode 0\n", n);
                break;

            case TRC_TRAP_ILL:
                fprintf (DST, "[KD11] TRAP %o: illegal instruction\n", n);
                break;
        }
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_TRAP);
        rec.trap = U16B (n);
        rec.cause = U16B (cause);
        fwrite (&rec, sizeof (rec), 1, file);
        fflush (file);
        limitFileSize ();
    }
}

template <> 
inline void TRACE::TRACEIrq<true> (int n, int type)
{
    TRACE_IRQ rec;

    if (!(flags & Irq) || flags & IgnoreBus)
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        switch (type)
        {
            case TRC_IRQ_OK:
                fprintf (DST, "[QBUS] interrupt request %o\n", n);
                break;
            case TRC_IRQ_FAIL:
                fprintf (DST, "[QBUS] interrupt request %o denied\n", n);
                break;
            case TRC_IRQ_SIG:
                fprintf (DST, "[QBUS] signaling irq %o\n", n);
                break;
        }
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_IRQ0);
        rec.trap = U16B (n);
        rec.type = U16B (type);
        fwrite (&rec, sizeof (rec), 1, file);
        fflush (file);
        limitFileSize ();
    }
}

template <> 
inline void TRACE::TRACEDLV11<true> (int channel, int type, u16 value)
{
    TRACE_DLV11 rec;

    if (!(flags & Dlv11))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        switch (type)
        {
            case TRC_DLV11_RX:
                fprintf (DST, "[DLV11J] CH%d: receive %06o [%c]\n", channel, value, value >= 0x20 ? value : '.');
                break;

            case TRC_DLV11_TX:
                fprintf (DST, "[DLV11J] CH%d: transmit %06o [%c]\n", channel, value, value >= 0x20 ? value : '.');
                break;
        }
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_DLV1);
        rec.channel = channel;
        rec.type = type;
        rec.value = U16B (value);
        fwrite (&rec, sizeof (rec), 1, file);
        fflush (file);
        limitFileSize ();
    }
}


template <> 
inline void TRACE::TRACERXV21Command<true> (int commit, int type, u16 rx2cs)
{
    TRACE_RXV21CMD rec;

    if (!(flags & RXV21Cmd))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        const char* name = rxv21_get_cmd_name (type);
        fprintf (DST, "[RXV21] Execute command: %s\n", name);
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_RX2C);
        rec.type = type;
        rec.commit = commit;
        rec.rx2cs = U16B (rx2cs);
        fwrite (&rec, sizeof (rec), 1, file);
        fflush (file);
        limitFileSize ();
    }
}

template <> 
inline void TRACE::TRACERXV21Step<true> (int type, int step, u16 rx2db)
{
    TRACE_RXV21STEP rec;

    if (!(flags & RXV21Step))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        const char* name = rxv21_get_cmd_name (type);
        fprintf (DST, "[RXV21] Processing command %s: step %d with data %06o\n",
            name, step, rx2db);
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_RX2S);
        rec.type = type;
        rec.step = step;
        rec.rx2db = U16B (rx2db);
        fwrite (&rec, sizeof (rec), 1, file);
        fflush (file);
        limitFileSize ();
    }
}

template <> 
inline void TRACE::TRACERXV21DMA<true> (int type, u16 rx2wc, u16 rx2ba)
{
    TRACE_RXV21DMA rec;

    if (!(flags & RXV21Dma))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        const char* name = rxv21_get_cmd_name (type);
        fprintf (DST, "[RXV21] DMA transfer [%s]: %06o words to %06o\n",
            name, rx2wc, rx2ba);
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_RX2D);
        rec.type = U16B (type);
        rec.rx2wc = U16B (rx2wc);
        rec.rx2ba = U16B (rx2ba);
        fwrite (&rec, sizeof (rec), 1, file);
        fflush (file);
        limitFileSize ();
    }
}

template <> 
inline void TRACE::TRACERXV21Error<true> (int type, u16 info)
{
    TRACE_RXV21ERR rec;

    if (!(flags & RXV21Error))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        const char* name = rxv21_get_error_name (type);
        switch (type)
        {
            case TRC_RXV21_WC_OVFL:
            case TRC_RXV21_TRACK_NO:
            case TRC_RXV21_SECT_NO:
                fprintf (DST, "[RXV21] Error: %s [%o/%d]\n",
                    name, info, info);
                break;

            default:
            case TRC_RXV21_DEN_ERR:
                fprintf (DST, "[RXV21] Error: %s\n", name);
                break;
        }
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_RX2E);
        rec.type = U16B (type);
        rec.info = U16B (info);
        fwrite (&rec, sizeof (rec), 1, file);
        fflush (file);
        limitFileSize ();
    }
}

template <> 
inline void TRACE::TRACERXV21Disk<true> (int type, int drive, int density, u16 rx2sa, u16 rx2ta)
{
    TRACE_RXV21DISK rec;

    if (!(flags & RXV21Disk))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        const char* name;
        switch (type)
        {
            case TRC_RXV21_READ:
                name = "read";
                break;

            case TRC_RXV21_WRITE:
                name = "write";
                break;

            case TRC_RXV21_WRITE_DD:
                name = "write (delete data)";
                break;

            default:
                name = "???";
                break;
        }
        fprintf (DST, "[RXV21] %s sector SEC=%d, TR=%d [drive %d, %s density]\n",
            name, rx2sa, rx2ta, drive,
            density ? "double" : "single");
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_RX2A);
        rec.type = U16B (type);
        rec.drive = drive;
        rec.density = density;
        rec.rx2sa = U16B (rx2sa);
        rec.rx2ta = U16B (rx2ta);
        fwrite (&rec, sizeof (rec), 1, file);
        fflush (file);
        limitFileSize ();
    }
}

template <> 
inline void TRACE::TRACERLV12Registers<true> (char const* msg, u16 rlcs, u16 rlba, u16 rlda,
    u16 rlmpr, u16 rlbae)
{
    TRACE_RLV12REGS rec;

    if (!(flags & RLV12))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        fprintf (DST,
            "[RLV12] %s: RLCS: %06o, RLBA: %06o, RLDA: %06o, RLMPR: %06o, RLBAE: %06o\n",
            msg, rlcs, rlba, rlda, rlmpr, rlbae);
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_RL2A);
        rec.rlcs = U16B (rlcs);
        rec.rlba = U16B (rlba);
        rec.rlda = U16B (rlda);
        rec.rlmpr = U16B (rlmpr);
        rec.rlbae = U16B (rlbae);

        u16 msgLength = strlen (msg);
        rec.length = U16B (msgLength);
        fwrite (&rec, sizeof (rec), 1, file);
        fwrite (msg, msgLength, 1, file);
        fflush (file);
        limitFileSize ();
    }
}

template <> 
inline void TRACE::TRACERLV12Command<true> (u16 command)
{
    TRACE_RLV12COMMAND rec;

    if (!(flags & RLV12))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        fprintf (DST, "[RLV12] command: %s\n", rlv12GetCommandName (command));
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_RL2C);
        rec.command = U16B (command);
        fwrite (&rec, sizeof (rec), 1, file);
        fflush (file);
        limitFileSize ();
    }
}

template <> 
inline void TRACE::TRACEDuration<true> (const char* msg, u32 duration)
{
    TRACE_DURATION rec;

    if (!(flags & Duration))
        return;

    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ traceFileMutex };

    if (flags & Print)
    {
        fprintf (DST, "[DURATION] %s: %u nanoseconds\n", msg, duration);
        fflush (DST);
    }

    if (flags & Write)
    {
        rec.magic = U32B (MAGIC_DURA);
        rec.durationCount = U32B (duration);

        u16 msgLength = strlen (msg);
        rec.length = U16B (msgLength);
        fwrite (&rec, sizeof (rec), 1, file);
        fwrite (msg, msgLength, 1, file);
        fflush (file);
        limitFileSize ();
    }
}

#endif

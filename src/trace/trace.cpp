#include "trace.h"

// Setting traceEnabled to false will optimize out all calls to trace
// functions.
static constexpr bool traceEnabled = true;

// The following objects and functions are defined in the global namespace. 
// This not ideal, but the alternatives - e.g. a singleton class or definition
// in a separate namespace - are even worse as it clobbers all source files
// with class or namespace references.
Trace trc;

// These functions are used in the simulator to write trace records to the
// tracefile. These are kept in order not having to rewrite them to the
// new-style trace file insertion operator calls.
// 
// ToDo: Functions to be merged with Trace class
void TRCOpen (const char* f)
{
    if (traceEnabled && trc.flags)
        trc.tracefileOut.open (f);
}

void TRCClose ()
{
    if (traceEnabled && trc.flags)
        trc.tracefileOut.close ();
}

// ToDo: The TRC functions should only write to the trace file when open
void TRCStep (u16* r, u16 psw, u16* insn)
{
    if (traceEnabled && (trc.flags & Trace::Category::Step))
        trc.tracefileOut << TraceRecord<TraceCpu> (r, psw, insn);
}

void TRCCPUEvent (CpuEventType type, u16 value) 
{
    if (traceEnabled && (trc.flags & Trace::Category::CpuEvent))
        trc.tracefileOut << TraceRecord<CpuEvent> (type, value);
}

void TRCBus (TraceBusType type, u16 address, u16 value) 
{
    if (traceEnabled && (trc.flags & Trace::Category::Bus) && 
            !(trc.flags & Trace::Category::IgnoreBus))
        trc.tracefileOut << TraceRecord<TraceBus> (type, address, value);
}

void TRCMemoryDump (u8* ptr, u16 address, u16 length) 
{
    if (traceEnabled && (trc.flags & Trace::Category::MemoryDump) && 
            !(trc.flags & Trace::Category::IgnoreBus))
        trc.tracefileOut << TraceRecord<MemoryDump> (ptr, address, length);
}

// Note: parameters are reversed!
void TRCIRQ (int vector, TraceIrqType type) 
{
    if (traceEnabled && (trc.flags & Trace::Category::Irq) && 
            !(trc.flags & Trace::Category::IgnoreBus))
        trc.tracefileOut << TraceRecord<TraceIrq> (type, vector);
}
// Note: parameters are reversed!
void TRCTrap (int vector, TraceTrapCause cause) 
{
    if (traceEnabled && (trc.flags & Trace::Category::Trap))
        trc.tracefileOut << TraceRecord<TraceTrap> (cause, vector);
}

void TRCDLV11 (TraceDLV11Type type, int channel, u16 value) 
{
    if (traceEnabled && (trc.flags & Trace::Category::DLV11))
        trc.tracefileOut << TraceRecord<TraceDLV11> (type, channel, value);
}

// This function takes a value take from the RX2CS register and casts it
// to the appropriate enum value.
void TRCRXV21CMD (int command, u16 rx2cs)
{
    if (traceEnabled && (trc.flags & Trace::Category::RXV21Cmd))
        trc.tracefileOut << 
            TraceRecord<RXV21Command> (static_cast<RXV21DiskCmd> (command), rx2cs);
}

// TRCRXV21CMDCommit calls can be replaced by calls to TRCRXV21CMD
//void TRCRXV21CMDCommit (int type, u16 rx2cs) 
//    { tracefileOut << TraceRecord<RXV21Command> (1, type, rx2cs); }
// The should be no TRCRXV21Step calls anymore
// void TRCRXV21Step (int type, int step, u16 rx2db) 
//    { throw "Unsupported trace record"; }

void TRCRXV21DMA (RXV21DiskCmd type, u16 rx2wc, u16 rx2ba) 
{
    if (traceEnabled && (trc.flags & Trace::Category::RXV21Dma))
        trc.tracefileOut << TraceRecord<RXV21Dma> (type, rx2wc, rx2ba);
}

void TRCRXV21Disk (RXV21DiskCmd type, int drive, int density, u16 rx2sa, u16 rx2ta) 
{
    if (traceEnabled && (trc.flags & Trace::Category::RXV21Disk))
        trc.tracefileOut << TraceRecord<RXV21Disk> (type, drive, density, rx2sa, rx2ta);
}

void TRCRXV21Error (RXV21ErrorType type, u16 info) 
{
    if (traceEnabled && (trc.flags & Trace::Category::RXV21Error))
        trc.tracefileOut << TraceRecord<RXV21Error> (type, info);
}

void TRCRLV12Registers (string msg, u16 rlcs, u16 rlba, u16 rlda,
        u16 rlmpr, u16 rlbae)
{
    if (traceEnabled && (trc.flags & Trace::Category::RLV12))
        trc.tracefileOut << TraceRecord<RLV12Registers> (msg, rlcs, rlba, rlda, rlmpr, rlbae);
}

void TRCRLV12Command (u16 command)
{
    if (traceEnabled && (trc.flags & Trace::Category::RLV12))
        trc.tracefileOut << TraceRecord<TraceRLV12Command> (command);
}

void TRCDuration (string msg, u32 durationCount) 
{
    if (traceEnabled && (trc.flags & Trace::Category::Duration))
        trc.tracefileOut << TraceRecord<TraceDuration> (msg, durationCount);
}

// ToDo: Merge IGNBUS functions with class Trace.
void TRCSETIGNBUS ()
{
    trc.flags |= Trace::Category::IgnoreBus;
}

void TRCCLRIGNBUS()
{
    trc.flags &= ~Trace::Category::IgnoreBus;
}

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
void Trace::open (const char* f)
{
    if (traceEnabled)
        tracefileOut.open (f);
}

void Trace::close ()
{
    if (traceEnabled && tracefileOut.is_open ())
        tracefileOut.close ();
}

// ToDo: The TRC functions should only write to the trace file when open
void Trace::step (u16* r, u16 psw, u16* insn)
{
    if (traceEnabled && (flags & Trace::Category::Step))
        tracefileOut << TraceRecord<CpuRecord> (r, psw, insn);
}

void Trace::cpuEvent (CpuEventRecordType type, u16 value) 
{
    if (traceEnabled && (flags & Trace::Category::CpuEvent))
        tracefileOut << TraceRecord<CpuEventRecord> (type, value);
}

void Trace::bus (BusRecordType type, u16 address, u16 value) 
{
    if (traceEnabled && (flags & Trace::Category::Bus) && 
            !(flags & Trace::Category::IgnoreBus))
        tracefileOut << TraceRecord<BusRecord> (type, address, value);
}

void Trace::memoryDump (u8* ptr, u16 address, u16 length) 
{
    if (traceEnabled && (flags & Trace::Category::MemoryDump) && 
            !(flags & Trace::Category::IgnoreBus))
        tracefileOut << TraceRecord<MemoryDumpRecord> (ptr, address, length);
}

// Note: parameters are reversed!
void Trace::irq (int vector, IrqRecordType type) 
{
    if (traceEnabled && (flags & Trace::Category::Irq) && 
            !(flags & Trace::Category::IgnoreBus))
        tracefileOut << TraceRecord<IrqRecord> (type, vector);
}
// Note: parameters are reversed!
void Trace::trap (int vector, TrapRecordType cause) 
{
    if (traceEnabled && (flags & Trace::Category::Trap))
        tracefileOut << TraceRecord<TrapRecord> (cause, vector);
}

void Trace::dlv11 (DLV11RecordType type, int channel, u16 value) 
{
    if (traceEnabled && (flags & Trace::Category::DLV11))
        tracefileOut << TraceRecord<DLV11Record> (type, channel, value);
}

// This function takes a value take from the RX2CS register and casts it
// to the appropriate enum value.
void Trace::rxv21Command (int command, u16 rx2cs)
{
    if (traceEnabled && (flags & Trace::Category::RXV21Cmd))
        tracefileOut << 
            TraceRecord<RXV21CommandRecord> (static_cast<RXV21DiskCmd> (command), rx2cs);
}

// TRCRXV21CMDCommit calls can be replaced by calls to rxv21Command
//void TRCRXV21CMDCommit (int type, u16 rx2cs) 
//    { tracefileOut << TraceRecord<RXV21CommandRecord> (1, type, rx2cs); }
// The should be no TRCRXV21Step calls anymore
// void TRCRXV21Step (int type, int step, u16 rx2db) 
//    { throw "Unsupported trace record"; }

void Trace::rxv21Dma (RXV21DiskCmd type, u16 rx2wc, u16 rx2ba) 
{
    if (traceEnabled && (flags & Trace::Category::RXV21Dma))
        tracefileOut << TraceRecord<RXV21DmaRecord> (type, rx2wc, rx2ba);
}

void Trace::rxv21Disk (RXV21DiskCmd type, int drive, int density, u16 rx2sa, u16 rx2ta) 
{
    if (traceEnabled && (flags & Trace::Category::RXV21Disk))
        tracefileOut << TraceRecord<RXV21DiskRecord> (type, drive, density, rx2sa, rx2ta);
}

void Trace::rxv21Error (RXV21ErrorRecordType type, u16 info) 
{
    if (traceEnabled && (flags & Trace::Category::RXV21Error))
        tracefileOut << TraceRecord<RXV21ErrorRecord> (type, info);
}

void Trace::rlv12Registers (string msg, u16 rlcs, u16 rlba, u16 rlda,
        u16 rlmpr, u16 rlbae)
{
    if (traceEnabled && (flags & Trace::Category::RLV12))
        tracefileOut << TraceRecord<RLV12RegistersRecord> (msg, rlcs, rlba, rlda, rlmpr, rlbae);
}

void Trace::rlv12Command (u16 command)
{
    if (traceEnabled && (flags & Trace::Category::RLV12))
        tracefileOut << TraceRecord<RLV12CommandRecord> (command);
}

void Trace::duration (string msg, u32 durationCount) 
{
    if (traceEnabled && (flags & Trace::Category::Duration))
        tracefileOut << TraceRecord<DurationRecord> (msg, durationCount);
}

// ToDo: Merge IGNBUS functions with class Trace.
void Trace::setIgnoreBus ()
{
    flags |= Trace::Category::IgnoreBus;
}

void Trace::clearIgnoreBus()
{
    flags &= ~Trace::Category::IgnoreBus;
}

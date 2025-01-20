#include "trace.h"

// Setting traceEnabled to false will optimize out all calls to trace
// functions.
static constexpr bool traceEnabled = true;

// The Trace object is defined in the global namespace. This not ideal, but
// the alternatives - e.g. a singleton class or definition in a separate
// namespace - are even worse as it clobbers all source files with class or
// namespace references.
Trace trace;

// Constructor initially deactivating the trace facility
Trace::Trace ()
    :
    flags_ {0}
{}

Trace::~Trace ()
{
    if (traceEnabled && tracefileOut_.is_open ())
        tracefileOut_.close ();
}

// Activate the trace facility by opening the trace file and setting the
// categories to be traced. Setting the trace category only when the
// trace file is opened avoids having to check at every write if the
// file is opened.
void Trace::activate (const char* filename, int flags)
{
    if (traceEnabled)
        tracefileOut_.open (filename);
    flags_ = flags;
}

bool Trace::isActive ()
{
    return flags_ != 0;
}

void Trace::cpuStep (u16* r, u16 psw, u16* insn)
{
    if (traceEnabled && (flags_ & Trace::Category::CpuStep))
        tracefileOut_ << TraceRecord<CpuStepRecord> (r, psw, insn);
}

void Trace::cpuEvent (CpuEventRecordType type, u16 value) 
{
    if (traceEnabled && (flags_ & Trace::Category::CpuEvent))
        tracefileOut_ << TraceRecord<CpuEventRecord> (type, value);
}

void Trace::bus (BusRecordType type, u32 address, u16 value) 
{
    if (traceEnabled && (flags_ & Trace::Category::Bus) && 
            !(flags_ & Trace::Category::IgnoreBus))
        tracefileOut_ << TraceRecord<BusRecord> (type, address, value);
}

void Trace::memoryDump (u8* ptr, u16 address, u16 length) 
{
    if (traceEnabled && (flags_ & Trace::Category::MemoryDump) && 
            !(flags_ & Trace::Category::IgnoreBus))
        tracefileOut_ << TraceRecord<MemoryDumpRecord> (ptr, address, length);
}

void Trace::irq (IrqRecordType type, int vector) 
{
    if (traceEnabled && (flags_ & Trace::Category::Irq) && 
            !(flags_ & Trace::Category::IgnoreBus))
        tracefileOut_ << TraceRecord<IrqRecord> (type, vector);
}

void Trace::trap (TrapRecordType cause, int vector) 
{
    if (traceEnabled && (flags_ & Trace::Category::Trap))
        tracefileOut_ << TraceRecord<TrapRecord> (cause, vector);
}

void Trace::dlv11 (DLV11RecordType type, int channel, u16 value) 
{
    if (traceEnabled && (flags_ & Trace::Category::DLV11))
        tracefileOut_ << TraceRecord<DLV11Record> (type, channel, value);
}

// This function takes a value take from the RX2CS register and casts it
// to the appropriate enum value.
void Trace::rxv21Command (int command, u16 rx2cs)
{
    if (traceEnabled && (flags_ & Trace::Category::RXV21Cmd))
        tracefileOut_ << 
            TraceRecord<RXV21CommandRecord> (static_cast<RXV21DiskCmd> (command), rx2cs);
}

void Trace::rxv21Dma (RXV21DiskCmd type, u16 rx2wc, u16 rx2ba) 
{
    if (traceEnabled && (flags_ & Trace::Category::RXV21Dma))
        tracefileOut_ << TraceRecord<RXV21DmaRecord> (type, rx2wc, rx2ba);
}

void Trace::rxv21Disk (RXV21DiskCmd type, int drive, int density, u16 rx2sa, u16 rx2ta) 
{
    if (traceEnabled && (flags_ & Trace::Category::RXV21Disk))
        tracefileOut_ << TraceRecord<RXV21DiskRecord> (type, drive, density, rx2sa, rx2ta);
}

void Trace::rxv21Error (RXV21ErrorRecordType type, u16 info) 
{
    if (traceEnabled && (flags_ & Trace::Category::RXV21Error))
        tracefileOut_ << TraceRecord<RXV21ErrorRecord> (type, info);
}

void Trace::rlv12Registers (string msg, u16 rlcs, u16 rlba, u16 rlda,
        u16 rlmpr, u16 rlbae)
{
    if (traceEnabled && (flags_ & Trace::Category::RLV12))
        tracefileOut_ << TraceRecord<RLV12RegistersRecord> (msg, rlcs, rlba, rlda, rlmpr, rlbae);
}

void Trace::rlv12Command (u16 command)
{
    if (traceEnabled && (flags_ & Trace::Category::RLV12))
        tracefileOut_ << TraceRecord<RLV12CommandRecord> (command);
}

void Trace::unibusMap (u32 inputAddress, BusAddress::Width width,
    u16 registerIndex, u16 low, u16 high, u32 physicalAddress)
{
    if (traceEnabled && (flags_ & Trace::Category::UnibusMap))
        tracefileOut_ << TraceRecord<UnibusMapRecord> (inputAddress, width,
            registerIndex, low, high, physicalAddress);
}

// This functions logs a time record in the logfile. It has to be passed a
// SimulatorClock::duration which can be used to log either a point in
// time (i.e. the simulated time elapsed since the start of the simulator)
// or a duration. To log the current time the function has to passed the
// result of a SimulatorClock::now ().time_since_epoch () call.
//
void Trace::clock (string msg, SimulatorClock::duration duration) 
{
    if (traceEnabled && (flags_ & Trace::Category::Clock))
        tracefileOut_ << TraceRecord<ClockRecord> (msg, duration);
}

void Trace::MmuApr (ActivePageRegisterSet const& aprSet)
{
    if (traceEnabled && (flags_ & Trace::Category::MMUAPR))
        tracefileOut_ << TraceRecord<KTF11_ARecord> (aprSet);
}

void Trace::setIgnoreBus ()
{
    flags_ |= Trace::Category::IgnoreBus;
}

void Trace::clearIgnoreBus()
{
    flags_ &= ~Trace::Category::IgnoreBus;
}

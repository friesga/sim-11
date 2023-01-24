#ifndef _TRACE_H_
#define _TRACE_H_

#include "types.h"
#include "cpueventrecord/cpueventrecord.h"
#include "busrecord/busrecord.h"
#include "memorydumprecord/memorydumprecord.h"
#include "cpusteprecord/cpusteprecord.h"
#include "traprecord/traprecord.h"
#include "irqrecord/irqrecord.h"
#include "rxv21diskrecord/rxv21diskrecord.h"
#include "rxv21commandrecord/rxv21commandrecord.h"
#include "rxv21dmarecord/rxv21dmarecord.h"
#include "rxv21errorrecord/rxv21errorrecord.h"
#include "dlv11record/dlv11record.h"
#include "rlv12registersrecord/rlv12registersrecord.h"
#include "rlv12commandrecord/rlv12commandrecord.h"
#include "durationrecord/durationrecord.h"

#include "tracefileoutstream/tracefileoutstream.h"
#include "tracefileinstream/tracefileinstream.h"

class Trace
{
    // ToDo: int flags to be changed to a Category
    int flags_ {};
    TracefileOutStream tracefileOut_;

public:
    // Definition of trace flags. These flags define the records to
    // be written to the tracefile.
    // 
    // IgnoreBus is a special flag; it temporarily overrides the
    // Bus, MemoryDump and Irq flags.
    //
    enum Category
    {
        IgnoreBus  = (1 << 0),
        CpuStep    = (1 << 1),      // CpuStepRecord
        CpuEvent   = (1 << 2),      // CpuEventRecord
        Bus		   = (1 << 3),      // BusRecord
        MemoryDump = (1 << 4),      // MemoryDumpRecord
        Trap	   = (1 << 5),      // TrapRecord
        Irq		   = (1 << 6),      // IrqRecord
        DLV11	   = (1 << 7),      // DLV11Record
        RXV21Cmd   = (1 << 8),      // RXV21DiskCmd
        RXV21Dma   = (1 << 9),      // RXV21DmaRecord
        RXV21Error = (1 << 10),     // RXV21ErrorRecord
        RXV21Disk  = (1 << 11),     // RXV21DiskRecord
        RLV12	   = (1 << 12),     // RLV12RegistersRecord, RLV12CommandRecord
        Duration   = (1 << 13)      // DurationRecord
    };

    Trace ();
    ~Trace ();

    // These functions are used in the simulator to write trace records to
    // the tracefile. These are kept in order not having to rewrite them
    // to the new-style trace file insertion operator calls.
    void activate (const char* filename, int flags);
    bool isActive ();
    void cpuStep (u16* r, u16 psw, u16* insn);
    void cpuEvent (CpuEventRecordType type, u16 value);
    void bus (BusRecordType type, u16 address, u16 value);
    void memoryDump (u8* ptr, u16 address, u16 length);
    void irq (IrqRecordType type, int vector);
    void trap (TrapRecordType cause, int vector);
    void dlv11 (DLV11RecordType type, int channel, u16 value);
    void rxv21Command (int command, u16 rx2cs);
    void rxv21Dma (RXV21DiskCmd type, u16 rx2wc, u16 rx2ba);
    void rxv21Disk (RXV21DiskCmd type, int drive, int density, u16 rx2sa, u16 rx2ta);
    void rxv21Error (RXV21ErrorRecordType type, u16 info);
    void rlv12Registers (string msg, u16 rlcs, u16 rlba, u16 rlda, u16 rlmpr, u16 rlbae);
    void rlv12Command (u16 command);
    void duration (string msg, u32 durationCount);
    void setIgnoreBus ();
    void clearIgnoreBus();
};


// External declaration for the Trace global object.
extern Trace trace;

#endif // !_TRACE_H_

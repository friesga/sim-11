#ifndef _TRACE_H_
#define _TRACE_H_

#include "types.h"
#include "cpueventrecord/cpueventrecord.h"
#include "busrecord/busrecord.h"
#include "memorydumprecord/memorydumprecord.h"
#include "cpurecord/cpurecord.h"
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
public:
    // Definition of trace flags. These flags define the records to
    // be written to the tracefile.
    // 
    // IgnoreBus is a special flag; it temporarily overrides the
    // Bus, MemoryDump and Irq flags.
    //
    // ToDo: Change Catgeory to enum Class
    // ToDo: Remove obsolete Category flags
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
        DLV11	   = (1 << 11),
        RXV21Cmd   = (1 << 12),
        RXV21Step  = (1 << 13),
        RXV21Dma   = (1 << 14),
        RXV21Error = (1 << 15),
        RXV21Disk  = (1 << 16),
        RLV12	   = (1 << 17),
        Duration   = (1 << 18)
    };

    TracefileOutStream tracefileOut;

    // ToDo: int flags to be changed to a Category
    // ToDo: flags to be made private 
    int flags {};

    void open (const char* f);
    void close ();
    void step (u16* r, u16 psw, u16* insn);
    void cpuEvent (CpuEventRecordType type, u16 value);
    void bus (BusRecordType type, u16 address, u16 value);
    void memoryDump (u8* ptr, u16 address, u16 length);
    void irq (int vector, IrqRecordType type);
    void trap (int vector, TrapRecordType cause);
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

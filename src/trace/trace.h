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
};


// External declaration for the Trace global object and the tracefile
// writing functions used in the simulator.
// ToDo: These functions should be defined as members of a Trace class.

extern Trace trc;

extern void TRCOpen (const char* f);
extern void TRCClose ();
extern void TRCStep (u16* r, u16 psw, u16* insn);
extern void TRCCPUEvent (CpuEventType type, u16 value);
extern void TRCBus (TraceBusType type, u16 address, u16 value);
extern void TRCMemoryDump (u8* ptr, u16 address, u16 length);
extern void TRCIRQ (int vector, TraceIrqType type);
extern void TRCTrap (int vector, TraceTrapCause cause);
extern void TRCDLV11 (TraceDLV11Type type, int channel, u16 value);
extern void TRCRXV21CMD (int command, u16 rx2cs);
extern void TRCRXV21DMA (RXV21DiskCmd type, u16 rx2wc, u16 rx2ba);
extern void TRCRXV21Disk (RXV21DiskCmd type, int drive, int density, u16 rx2sa, u16 rx2ta);
extern void TRCRXV21Error (RXV21ErrorType type, u16 info);
extern void TRCRLV12Registers (string msg, u16 rlcs, u16 rlba, u16 rlda, u16 rlmpr, u16 rlbae);
extern void TRCRLV12Command (u16 command);
extern void TRCDuration (string msg, u32 durationCount);

extern void TRCSETIGNBUS ();
extern void TRCCLRIGNBUS();

#endif // !_TRACE_H_

#ifndef _TRACE_H_
#define _TRACE_H_

#if 0
// Definition of functions to write the trace file in big endian order.
// Currently not used.
#ifdef WIN32
// We can safely asseume that all Win32 systems are little endian.
// The following functions will be inlined by the compiler.
u16 U16B (u16 x) {return _byteswap_ushort (x);}
u32 U32B (u32 x) {return _byteswap_ulong (x);}
u64 U64B (u64 x) {return _byteswap_uint64 (x);}
u16 U16L (u16 x) {return x;}
u32 U32L (u32 x) {return x;}
u64 U64L (u64 x) {return x;}
#else // WIN32
// Use byte order as defined by gcc
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
constexpr u16 U16B (u16 x) {return x;}
constexpr u32 U32B (u32 x) {return x;}
constexpr u64 U64B (u64 x) {return x;}
constexpr u16 U16L (u16 x) {return __builtin_bswap16(x);}
constexpr u32 U32L (u32 x) {return __builtin_bswap32(x);}
constexpr u64 U64L (u64 x) {return __builtin_bswap64(x);}
#else	// __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
constexpr u16 U16B (u16 x) {return __builtin_bswap16(x);}
constexpr u32 U32B (u32 x) {return __builtin_bswap32(x);}
constexpr u64 U64B (u64 x) {return __builtin_bswap64(x);}
constexpr u16 U16L (u16 x) {return x;}
constexpr u32 U32L (u32 x) {return x;}
constexpr u64 U64L (u64 x) {return x;}
#endif	// __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#endif // WIN32
#endif // 0

#include "types.h"
#include "cpuevent/cpuevent.h"
#include "tracebus/tracebus.h"
#include "memorydump/memorydump.h"
#include "tracecpu/tracecpu.h"
#include "tracetrap/tracetrap.h"
#include "traceirq/traceirq.h"
#include "rxv21disk/rxv21disk.h"
#include "rxv21command/rxv21command.h"
#include "rxv21dma/rxv21dma.h"
#include "rxv21error/rxv21error.h"
#include "tracedlv11/tracedlv11.h"
#include "rlv12registers/rlv12registers.h"
#include "rlv12command/rlv12command.h"
#include "traceduration/traceduration.h"

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

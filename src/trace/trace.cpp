#include "trace.h"

#include <stdio.h>
#include <string.h>


// The following variable and functions are defined in the global namespace. 
// This not ideal, but the alternatives - e.g. a singleton class or definition
// in a separate namespace - are even worse as it clobbers all source files
// with class or namespace references.
TRACE trc;

// Convenience functions
void TRCOpen (const char* f)
    { trc.TRACEOpen<traceEnabled> (f); }
void TRCClose()
    { trc.TRACEClose<traceEnabled> (); }
void TRCStep(u16* r, u16 psw, u16* insn) 
    { trc.TRACEStep<traceEnabled> (r, psw, insn); }
void TRCCPUEvent(int type, u16 value) 
    { trc.TRACECPUEvent<traceEnabled> (type, value); }
void TRCBus(u16 type, u16 address, u16 value) 
    { trc.TRACEBus<traceEnabled> (type, address, value); }
void TRCMemoryDump(u8* ptr, u16 address, u16 length) 
    { trc.TRACEMemoryDump<traceEnabled> (ptr, address, length); }
void TRCIRQ(int n, int type) 
    { trc.TRACEIrq<traceEnabled> (n, type); }
void TRCTrap(int n, int cause) 
    { trc.TRACETrap<traceEnabled> (n, cause); }
void TRCDLV11(int channel, int type, u16 value) 
    { trc.TRACEDLV11<traceEnabled> (channel, type, value); }
void TRCRXV21CMD(int type, u16 rx2cs) 
    { trc.TRACERXV21Command<traceEnabled> (0, type, rx2cs); }
void TRCRXV21CMDCommit(int type, u16 rx2cs) 
    { trc.TRACERXV21Command<traceEnabled> (1, type, rx2cs); }
void TRCRXV21Step(int type, int step, u16 rx2db) 
    { trc.TRACERXV21Step<traceEnabled> (type, step, rx2db); }
void TRCRXV21DMA(int type, u16 rx2wc, u16 rx2ba) 
    { trc.TRACERXV21DMA<traceEnabled> (type, rx2wc, rx2ba); }
void TRCRXV21Disk(int type, int drive, int density, u16 rx2sa, u16 rx2ta) 
    { trc.TRACERXV21Disk<traceEnabled> (type, drive, density, rx2sa, rx2ta); }
void TRCRXV21Error(int type, u16 info) 
    { trc.TRACERXV21Error<traceEnabled> (type, info); }

// Generate the trace file in chunks of 10^6 records to be able to inspect
// large traces (in the order of more than 100*10^6 records). The name of
// the new trace file is the original file name, appended with an underscore
// and a sequence number.
void TRACE::limitFileSize ()
{
    static size_t fileSequenceNumber{ 0 };
    static size_t recordCounter{ 0 };

    if (recordCounter++ > 1'000'000)
    {
        // Change to next file
        TRACEClose ();
        std::string fileName = baseFileName + "_" +
            std::to_string (fileSequenceNumber++);
        TRACEOpen (fileName.c_str ());
        recordCounter = 0;
    }
}

const char* TRACE::get_trap_name (int n)
{
    switch (n)
    {
        case 0004: /* Bus error, Time out */
            return "Bus error / time out";
            break;
        case 0010: /* Reserved */
            return "Reserved instruction";
        case 0014: /* BPT trap instruction, T bit */
            return "BPT trap instruction / T bit";
        case 0020: /* IOT executed */
            return "IOT executed";
        case 0024: /* Power fail / restart */
            return "Power fail / restart";
        case 0030: /* EMT executed */
            return "EMT executed";
        case 0034: /* TRAP executed */
            return "TRAP executed";
        case 0060: /* Console input device */
            return "Console input device";
        case 0064: /* Console output device */
            return "Console output device";
        case 0100: /* External event line interrupt */
            return "External event line interrupt";
        case 0244: /* FIS trap */
            return "FIS trap";
        default:
            return NULL;
    }
}

const char* TRACE::rxv21_get_cmd_name (int func)
{
    switch (func)
    {
        case TRC_RXV21_FILL:
            return "Fill Buffer";

        case TRC_RXV21_EMPTY:
            return "Empty Buffer";

        case TRC_RXV21_WRITE:
            return "Write Sector";

        case TRC_RXV21_READ:
            return "Read Sector";

        case TRC_RXV21_SET_MD:
            return "Set Media Density";

        case TRC_RXV21_STATUS:
            return "Read Status";

        case TRC_RXV21_WRITE_DD:
            return "Write Deleted Data Sector";

        case TRC_RXV21_READ_ERR:
            return "Read Error Code";

        default:
            return "unknown";
    }
}

const char* TRACE::rxv21_get_error_name (int type)
{
    switch (type)
    {
        case TRC_RXV21_WC_OVFL:
            return "Word Count overflow";

        case TRC_RXV21_DEN_ERR:
            return "Density mismatch";

        case TRC_RXV21_TRACK_NO:
            return "Tried to access a track greater than 76";

        case TRC_RXV21_SECT_NO:
            return "Desired sector could not be found after looking at 52 headers (2 revolutions)";

        default:
            return "unknown";
    }
}

const char* TRACE::rlv12GetCommandName (u16 command)
{
    const char* commandName[] =
    {
        "No-Op/Maintenance",
        "Write Check",
        "Get Status",
        "Seek",
        "Read Header",
        "Write Data",
        "Read Data",
        "Read Data Without Header Check"
    };

    return commandName[command];
}

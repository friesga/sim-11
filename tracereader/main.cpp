#include "trace/trace.h"

#include <iostream>

template <typename T>
void readAndPrintRecord (TracefileInStream &tracefile)
{
    TraceRecord<T> record;

    tracefile >> record;
    std::cout << record;
}

void processTraceRecord(Magic magic, TracefileInStream &tracefile)
{
    switch (magic)
    {
       	case Magic::CPU0:
            readAndPrintRecord<CpuStepRecord> (tracefile);
            break;

	    case Magic::CPUZ:
            throw "Unsupported record: MAGIC_CPUZ\n";

	    case Magic::CPU1:
            readAndPrintRecord<CpuEventRecord> (tracefile);
            break;

	    case Magic::TIME:
            readAndPrintRecord<TimeRecord> (tracefile);
            break;

        case Magic::BUS0:
            readAndPrintRecord<BusRecord> (tracefile);
            break;

        case Magic::BUS1:
            readAndPrintRecord<MemoryDumpRecord> (tracefile);
            break;

        case Magic::TRAP:
            readAndPrintRecord<TrapRecord> (tracefile);
            break;

        case Magic::IRQ0:
            readAndPrintRecord<IrqRecord> (tracefile);
            break;

        case Magic::RX2A:
            readAndPrintRecord<RXV21DiskRecord> (tracefile);
            break;

        case Magic::RX2C:
            readAndPrintRecord<RXV21CommandRecord> (tracefile);
            break;

        case Magic::RX2D:
            readAndPrintRecord<RXV21DmaRecord> (tracefile);
            break;

        case Magic::RX2E:
            readAndPrintRecord<RXV21ErrorRecord> (tracefile);
            break;

        case Magic::RX2S:
            // The RXV21 implementation doesn't use steps anymore
            throw "Unsupported record: MAGIC_RX2S\n";

        case Magic::DLV1:
            readAndPrintRecord<DLV11Record> (tracefile);
            break;

        case Magic::RL2A:
            readAndPrintRecord<RLV12RegistersRecord> (tracefile);
            break;

        case Magic::RL2C:
            readAndPrintRecord<RLV12CommandRecord> (tracefile);
            break;

        case Magic::MMU0:
            readAndPrintRecord<KTF11_ARecord> (tracefile);
            break;

        default:
            throw string("Unknown magic");
    }
}

int main (int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <trace file>\n";
		return 0;
	}

	// Open the trace file. The constructor will check the existence
    // and header of the file and will throw an exception if it's not a
    // valid tracefile.
    TracefileInStream tracefile (argv[1]);

    TraceRecord<RecordHeader> recordHeader;

    while (tracefile >> recordHeader)
        processTraceRecord (recordHeader.magic(), tracefile);
}
#include "trace/trace.h"

#include <iostream>
#include <fstream>
#include <memory>

using namespace std;

static int const headerSize = 6;

// Definition of trace records
TRACE_CPU traceCpu;
TRACE_CPUZ traceCpuZ;
TRACE_CPUEVT traceCpuEvt;
TRACE_BUS traceBus;
TRACE_MEMDUMP traceMemDump;
TRACE_TRAP traceTrap;
TRACE_IRQ traceIrq;
TRACE_RXV21DISK traceRxv21Disk;
TRACE_RXV21CMD traceRxv21Cmd;
TRACE_RXV21DMA traceRxv21Dma;
TRACE_RXV21ERR traceRxv21Err;
TRACE_RXV21STEP traceRxv21Step;
TRACE_DLV11 traceDlv11;

bool isHeader (char *buffer)
{
	char header[6] = { 'X', 'T', 'R', 'C', 0, 65 };

	for (size_t index = 0; index < headerSize; ++index)
		if (buffer[index] != header[index])
			return false;

	return true;
}

long operator""_KB( unsigned long long const x )
{ 
	return 1024L * x;
}

int main (int argc, char **argv)
{
	int const oneByte = 1;
	int const maxRecordSize = 40;
	char buffer [maxRecordSize];
	ifstream traceFile;
	TRACE trace;
	size_t const maxMemorySize = 64_KB;
	
	trace.flags = TRACEF_PRINT;

	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <trace file>\n";
		return 0;
	}

	// Open trace file and check it succeeded
	traceFile.open (argv[1], ifstream::binary);
	if (!traceFile.is_open())
	{
		std::cout << "Cannot open " << argv[1] << '\n' ;
		return 1;
	}

	// Read file header and check the file is a valid trace file
	traceFile.read (reinterpret_cast<char *> (&buffer), headerSize);

	if ((traceFile.gcount() != headerSize) || !isHeader (buffer))
	{
		std::cout << argv[1] << " is not a valid trace file\n";
		return 1;
	}

	// Allocate a buffer for memory dumps
	u8 *memoryDump = new u8[maxMemorySize];

	while (traceFile)
	{
		uint32_t magic;
		size_t const magicSize = sizeof (magic);

		// Read record  magic 
		traceFile.read (reinterpret_cast<char *> (&magic), magicSize);

		if (!traceFile.good() || traceFile.gcount() != magicSize)
		{
			cout << "Read error on " << argv[1] << '\n';
			delete memoryDump;
			return 1;
		}

		// Process record
		switch (U32B(magic))
		{
			case MAGIC_CPU0:
				traceFile.read (
					reinterpret_cast<char *> (&traceCpu) + magicSize,
					sizeof (traceCpu) - magicSize);

				for (size_t index = 0; index < 8; ++index)
					traceCpu.r[index] = U16B(traceCpu.r[index]);

				for (size_t index = 0; index < 3; ++index)
					traceCpu.insn[index] = U16B(traceCpu.insn[index]);

				TRACEStep (&trace, 
					traceCpu.r, 
					U16B(traceCpu.psw), 
					traceCpu.insn);
				break;
				
			case MAGIC_CPUZ:
				traceFile.read (
					reinterpret_cast<char *> (&traceCpuZ) + magicSize,
					sizeof (traceCpuZ) - magicSize);

				cout << "Unsupported record: MAGIC_CPUZ\n";
				break;

			case MAGIC_CPU1:
				traceFile.read (
					reinterpret_cast<char *> (&traceCpuEvt) + magicSize,
					sizeof (traceCpuEvt) - magicSize);

				TRACECPUEvent (&trace, U16B(traceCpuEvt.type), U16B(traceCpuEvt.value));
				break;

			case MAGIC_BUS0:
				traceFile.read (
					reinterpret_cast<char *> (&traceBus) + magicSize,
					sizeof (traceBus) - magicSize);

				TRACEBus (&trace, 
					U16B(traceBus.type), 
					U16B(traceBus.addr),
					U16B(traceBus.value));
				break;

			case MAGIC_BUS1:
				// A MAGIC_BUS1 record is followed by a memory dump of the
				// in the record specified length
				traceFile.read (
					reinterpret_cast<char *> (&traceMemDump) + magicSize,
					sizeof (traceMemDump) - magicSize);

				traceFile.read (reinterpret_cast<char *> (memoryDump),
					U16B(traceMemDump.len));

				TRACEMemoryDump (&trace, 
					memoryDump, 
					U16B(traceMemDump.addr),
					U16B(traceMemDump.len));
				break;

			case MAGIC_TRAP:
				traceFile.read (
					reinterpret_cast<char *> (&traceTrap) + magicSize,
					sizeof (traceTrap) - magicSize);

				TRACETrap (&trace, 
					U16B(traceTrap.trap), 
					U16B(traceTrap.cause));
				break;

			case MAGIC_IRQ0:
				traceFile.read (
					reinterpret_cast<char *> (&traceIrq) + magicSize,
					sizeof (traceIrq) - magicSize);

				TRACEIrq (&trace, 
					U16B(traceIrq.trap), 
					U16B(traceIrq.type));
				break;

			case MAGIC_RX2A:
				traceFile.read (
					reinterpret_cast<char *> (&traceRxv21Disk) + magicSize,
					sizeof (traceRxv21Disk) - magicSize);

				TRACERXV21Disk (&trace, 
					U16B(traceRxv21Disk.type),
					U16B(traceRxv21Disk.drive),
					U16B(traceRxv21Disk.density),
					U16B(traceRxv21Disk.rx2sa),
					U16B(traceRxv21Disk.rx2ta));
				break;

			case MAGIC_RX2C:
				traceFile.read (
					reinterpret_cast<char *> (&traceRxv21Cmd) + magicSize,
					sizeof (traceRxv21Cmd) - magicSize);

				TRACERXV21Command (&trace, 
					traceRxv21Cmd.commit, 
					traceRxv21Cmd.type, 
					U16B(traceRxv21Cmd.rx2cs));
				break;

			case MAGIC_RX2D:
				traceFile.read (
					reinterpret_cast<char *> (&traceRxv21Dma) + magicSize,
					sizeof (traceRxv21Dma) - magicSize);

				TRACERXV21DMA (&trace, 
					U16B(traceRxv21Dma.type),
					U16B(traceRxv21Dma.rx2wc),
					U16B(traceRxv21Dma.rx2ba));
				break;

			case MAGIC_RX2E:
				traceFile.read (
					reinterpret_cast<char *> (&traceRxv21Err) + magicSize,
					sizeof (traceRxv21Err) - magicSize);

				TRACERXV21Error (&trace, 
					U16B(traceRxv21Err.type),
					U16B(traceRxv21Err.info));
				break;

			case MAGIC_RX2S:
				traceFile.read (
					reinterpret_cast<char *> (&traceRxv21Step) + magicSize,
					sizeof (traceRxv21Step) - magicSize);

				TRACERXV21Step (&trace, 
					traceRxv21Step.type, 
					traceRxv21Step.step, 
					U16B(traceRxv21Step.rx2db));
				break;

			case MAGIC_DLV1:
				traceFile.read (
					reinterpret_cast<char *> (&traceDlv11) + magicSize,
					sizeof (traceDlv11) - magicSize);

				TRACEDLV11 (&trace, 
					traceDlv11.channel, 
					traceDlv11.type,
					U16B(traceDlv11.value));
				break;

			default:
				cout << "Unknown magic: " << U32B(magic) << '\n';
				delete memoryDump;
				return 1;
		}
	}

	delete memoryDump;
	return 0;
}
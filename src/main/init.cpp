#include "main.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"

void Main::init(int argc, char const **argv)
{
    	// Open log file
	Logger::init ("sim-11.log");

	// Get command line options
	CmdLineOptions::processOptions (argc, argv);

	if (CmdLineOptions::get().trace_file) 
	{
		TRCINIT(CmdLineOptions::get().trace_file);
		if (CmdLineOptions::get().compress) 
		{
			trc.flags |= TRACEF_COMPRESS;
		}
	}

	// Select the events to be traced and the way the trace output has
	// to be generated. The flags can only be specified if tracing has been
	// enabled.
	if (trc.file != nullptr)
	{
		trc.flags |= TRACEF_RLV12;
		trc.flags |= TRACEF_STEP;
	}
}
#include "main.h"
#include "trace/trace.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "configdata/configprocessor/configprocessor.h"
#include "logger/logger.h"

#include <iostream>

// Get rid of SDL's main function
#undef main

// Definition of Main constructor to initialize the data members
Main::Main (int argc, char const **argv)
	: 
	msv11_ {nullptr},
	rxv21_ {nullptr},
	rlv12_ {nullptr},
	dlv11_ {nullptr},
	bdv11_ {nullptr},
	ba11_n_ {nullptr},
	console_ {nullptr},
	running_ {false}
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

Main::~Main ()
{
	if(CmdLineOptions::get().trace_file)
	{
		TRCFINISH();
	}
}

int main (int argc, char const **argv)
try
{
	Main simulator (argc, argv);

	// Configure the devices with the parameters specified in the
	// configuration file
	// ToDo: Change CmdLineOptions singleton to a Main class data member
	simulator.configureDevices (CmdLineOptions::get().config_file);
	
	if (CmdLineOptions::get().load_file) 
		simulator.loadFile (CmdLineOptions::get().load_file);

	// Run the simulator
	simulator.run();

	// Necessary for linux to get the command line prompt on the next line
	std::cout << std::endl;
}
catch (std::string msg)
{
	std::cerr << msg << '\n';
	return 1;
}

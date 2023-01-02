#include "main.h"
#include "trace/trace.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "configdata/configprocessor/configprocessor.h"
#include "logger/logger.h"

#include <iostream>

// Get rid of SDL's main function
#undef main

// Definition of Main constructor to initialize the data members
Main::Main (CmdLineOptions const &cmdLineOptions)
	: 
	msv11_ {nullptr},
	rxv21_ {nullptr},
	rlv12_ {nullptr},
	dlv11_ {nullptr},
	bdv11_ {nullptr},
	ba11_n_ {nullptr},
	console_ {nullptr},
	running_ {false},
	cmdLineOptions_ {cmdLineOptions}
{
	// Open log file
	Logger::init ("sim-11.log");

	if (cmdLineOptions_.trace_file) 
	{
		TRCINIT(cmdLineOptions_.trace_file);
		if (cmdLineOptions_.compress) 
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
	if (trc.file)
		TRCFINISH();
}

int main (int argc, char **argv)
try
{
	// Get command line options
	CmdLineOptions cmdLineOptions (argc, argv);

	// Create a simulator
	Main simulator {cmdLineOptions};

	// Configure the devices with the parameters specified in the
	// configuration file
	simulator.configureDevices ();
	
	if (cmdLineOptions.load_file) 
		simulator.loadFile ();

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

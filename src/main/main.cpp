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
	cmdLineOptions_ {cmdLineOptions},
	running_ {false}
{
	// Open log file
	Logger::init ("sim-11.log");

	// Activate the tracing facility when specified on the command line
	if (cmdLineOptions_.trace_file) 
		trace.activate (cmdLineOptions_.trace_file, 
			Trace::Category::RLV12 | Trace::Category::CpuStep);
}

int main (int argc, char const **argv)
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

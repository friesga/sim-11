#include "lsi11/lsi11.h"
#include "trace/trace.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"

#include <iostream>

// Get rid of SDL's main function
#undef main

int main (int argc, char const **argv)
try
{
	// Open log file
	Logger::init ("sim-11.log");

	// Get command line options
	CmdLineOptions cmdLineOptions (argc, argv);

	// Activate the tracing facility when specified on the command line
	if (cmdLineOptions.trace_file) 
		trace.activate (cmdLineOptions.trace_file, 
			Trace::Category::RLV12 | Trace::Category::CpuStep);

	// Create a lsi11
	LSI11 lsi11 {cmdLineOptions};

	// Configure the devices with the parameters specified in the
	// configuration file
	lsi11.configureDevices ();

	// Run the lsi11
	lsi11.run();

	// Necessary for linux to get the command line prompt on the next line
	std::cout << std::endl;
}
catch (std::string msg)
{
	std::cerr << msg << '\n';
	return 1;
}

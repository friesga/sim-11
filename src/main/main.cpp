#include "main.h"
#include "trace/trace.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "configdata/configprocessor/configprocessor.h"
#include "logger/logger.h"

#include <iostream>

// Get rid of SDL's main function
#undef main

int main (int argc, char const **argv)
try
{
	Main simulator;

	simulator.init (argc, argv);

	// Configure the devices with the parameters specified in the
	// configuration file
	// ToDo: Change CmdLineOptions singleton to a Main class data member
	simulator.configureDevices (CmdLineOptions::get().config_file);
	
	if (CmdLineOptions::get().load_file) 
		simulator.loadFile (CmdLineOptions::get().load_file);

	// Run the simulator
	simulator.run();

	std::cout << std::endl;

	if(CmdLineOptions::get().trace_file)
	{
		TRCFINISH();
	}
}
catch (std::string msg)
{
	std::cerr << msg << '\n';
	return 1;
}

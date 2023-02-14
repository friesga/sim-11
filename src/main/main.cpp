#include "lsi11/lsi11.h"
#include "trace/trace.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"
#include "configdata/iniprocessor/iniprocessor.h"

#include <iostream>

// Get rid of SDL's main function
#undef main

vector<shared_ptr<DeviceConfig>> createSystemConfig (const char* const &configFile)
{
    IniProcessor configProcessor;
    iniparser::File ft;

    if (!ft.load (configFile))
        throw "Error: cannot open file " + string (configFile);

    try
    {
        configProcessor.process (ft);
    }
    catch (std::invalid_argument const& except)
    {
        throw "Error in configuration file: " + string (except.what ());
    }

	return configProcessor.getSystemConfig ();
}

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

	// If a configuration file is specified create the system configuration from
	// that file and configure the lsi11 with the devices and parameters as
	// specified in that file. If no file is specified use the default
	// configuration.
	if (cmdLineOptions.config_file)
		lsi11.configureDevices (createSystemConfig (cmdLineOptions.config_file));
	else
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

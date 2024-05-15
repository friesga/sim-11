#include "pdp11/pdp11.h"
#include "trace/trace.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "sdl/sdlwindow/sdlwindow.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <iostream>

// Get rid of SDL's main function
#undef main

vector<DeviceConfig> createSystemConfig (const char* const &configFile)
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
			Trace::Category::CpuStep |
			Trace::Category::RLV12 |
			Trace::Category::Clock);

	// Create the window for the panels
	SDLWindow sdlWindow {"PDP-11 Console", 100, 100, 750, 200};

	// Create a pdp-11 with configured (or default) devices.
	// The devices are created after creation of the window to make sure the
	// devices are destructed befor destruction of the SDLWindow. This prevents
	// a segmentation fault that can occur if the devices access the window's
	// panels.
	PDP_11 pdp11 {cmdLineOptions};

	// If a configuration file is specified create the system configuration from
	// that file and configure the pdp-11 with the devices and parameters as
	// specified in that file. If no file is specified use the default
	// configuration.
	if (cmdLineOptions.config_file)
		pdp11.configureDevices (createSystemConfig (cmdLineOptions.config_file),
			&sdlWindow);
	else
		pdp11.configureDevices (&sdlWindow);

	// Start the pdp-11. This starts the processor
	pdp11.run();

	// Start rendering the window and processing events
	sdlWindow.handler ();

	SimulatorClock::halt ();

	// Necessary for linux to get the command line prompt on the next line
	std::cout << std::endl;
}
catch (std::string msg)
{
	std::cerr << msg << '\n';
	return 1;
}

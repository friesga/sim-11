#include "configdata/systemconfig/systemconfig.h"
#include "panel.h"
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

SystemConfig createSystemConfig (const char* const &configFile)
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

	// A H9642 cabinet is 19" wide and has an internal height of 35".
	// This height corresponds to 20 rack units. The aspect ratio of a H9642
	// cabinet therefore is 19/35 = 0.54.
	static const double h9642AspectRatio = 19.0 / 35.0;
	static const int windowWidth = 500;

	// Create the window with the aspect ratio of a H9642 cabinet for the
	// panels. Set the window initially hidden as configuring the devices may
	// throw an exception.
	SDLWindow sdlWindow {"PDP-11 Console", {100, 100, windowWidth,
		static_cast<int> (windowWidth / h9642AspectRatio)},
		{Window::Flag::WindowHidden}};

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
	{
		SystemConfig systemConfig = createSystemConfig (cmdLineOptions.config_file);
		pdp11.configureDevices (systemConfig, &sdlWindow);
	}
	else
		pdp11.configureDevices (&sdlWindow);

	// Configuration succeeded and required size of the window is known so it
	// can be shown now.
	sdlWindow.show ();

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

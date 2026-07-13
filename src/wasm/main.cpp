#include "configdata/systemconfig/systemconfig.h"
#include "panel.h"
#include "pdp11/pdp11.h"
#include "trace/trace.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "sdl/video/sdlwindow/sdlwindow.h"
#include "chrono/simulatorclock/simulatorclock.h"
#include "ba11/ba11_n/ba11_n.h"
#include "configdata/ba11/ba11n/ba11nconfig/ba11nconfig.h"

#include <SDL.h>
#include <SDL_image.h>
#include <emscripten.h>
#include <html5.h>
#include <iostream>
#include <memory>

static const double h9642AspectRatio = 19.0 / 35.0;
static const int windowWidth = 500;
const RackUnit unitHeight {3_ru};
Frame<float> ba11_nFrontFrame {0, 0, 1.0, 1.0};

std::unique_ptr<SDLWindow> sdlWindow;

SystemConfig createSystemConfig (const char* const& configFile)
{
    IniProcessor configProcessor;
    iniparser::File ft;

    if (!ft.load (configFile))
        throw invalid_argument ("Error: cannot open file " +
            string (configFile));

    try
    {
        configProcessor.process (ft);
    }
    catch (std::invalid_argument const& except)
    {
        throw invalid_argument ("Error in configuration file: " +
            string (except.what ()));
    }

    return configProcessor.getSystemConfig ();
}

static void renderLoop ()
{
    sdlWindow->wasmLoop ();
}

int main (int argc, char const** argv)
{
    // Get command line options
    CmdLineOptions cmdLineOptions (argc, argv);

    SDL_Init (SDL_INIT_VIDEO);
    IMG_Init (IMG_INIT_PNG);

    emscripten_set_canvas_element_size ("#canvas", 500, 1000);
    emscripten_set_element_css_size ("#canvas", 500, 1000);

    sdlWindow = std::make_unique<SDLWindow> ("sim-11", Frame {100, 100, windowWidth,
        static_cast<int> (windowWidth / h9642AspectRatio)},
        std::set<Window::Flag> {Window::Flag::WindowHidden});

    Qbus* bus = new Qbus ();

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
        pdp11.configureDevices (systemConfig, sdlWindow.get ());
    }
    else
    {
        pdp11.configureDevices (sdlWindow.get ());
    }

    // Configuration succeeded and required size of the window is known so it
    // can be shown now.
    sdlWindow->show ();

    // Start the pdp-11. This starts the processor
    pdp11.run ();

    // Argument 3 (set_infinite_loop) is set to true to prevent the browser
    // from timing out the main loop after a certain amount of time. This is
    // necessary as the main loop will run indefinitely until the user closes
    // the page, and we don't want the browser to think that the page has
    // become unresponsive.
    emscripten_set_main_loop (renderLoop, 0, 1);
}

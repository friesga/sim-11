#include "rk05.h"

using std::shared_ptr;

RK05::RK05 (Bus* bus, PDP11Peripheral* controller, Window* window,
    shared_ptr<RK05Config> configPtr)
    : 
    Unit (controller),
    bus_ {bus},
    controller_ {controller}
{
    // In case the constructor is called in the unit tests, the window is
    // not available.
    if (window != nullptr)
        createBezel (window, configPtr);
}

void RK05::runLoadSwitchClicked (Button::State state)
{}

void RK05::wtprotSwitchClicked (Button::State state)
{
}
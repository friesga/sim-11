#include "rk05.h"

#include <functional>

using std::bind;
using std::placeholders::_1;

void RK05::createBezel (Window* window, shared_ptr<RK05Config> rk05Config)
{
    Panel* panel = window->createPanel (rk05Config->cabinetPosition,
        RK05Config::unitHeight);
    panel->createFront ("resources/RK05-front.png", {0, 0, 1.0, 1.0});

    // PWR indicator, default on
    pwrIndicator_ = panel->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::On, pwrIndicatorFrame);

    // RDY indicator, default off
    rdyIndicator_ = panel->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, readyIndicatorFrame);

    // ONCYL indicator, default off
    oncylIndicator_ = panel->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, oncylIndicatorFrame);

    // WTPROT indicator, default off
    wtprotIndicator_ = panel->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, wtprotIndicatorFrame);

    // LOAD indicator, default on
    loadIndicator_ = panel->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::On, loadIndicatorFrame);

    // WT indicator, default off
    wtIndicator_ = panel->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, wtIndicatorFrame);

    // RD indicator, default off
    rdIndicator_ = panel->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, rdIndicatorFrame);

    // FAULT indicator, default off
    faultIndicator_ = panel->createIndicator (
        "resources/FAULT off.png",
        "resources/FAULT on.png",
        Indicator::State::Off, faultIndicatorFrame);

    // RUN/LOAD switch, initial state up. The RUN/LOAD switch is a rocker
    // switch with two latched positions.
    runLoadSwitch_ = panel->createLatchingButton (
        "resources/rocker switch up.png",
        "resources/rocker switch down.png",
        Button::TwoPositionsState::Off,
        bind (&RK05::runLoadSwitchClicked, this, _1),
        runLoadSwitchFrame);

    // WTPROT switch, initial state up. The WTPROT switch is a rocker switch,
    // spring loaded in the off position.
    wtprotSwitch_ = panel->createMomentaryButton (
        "resources/rocker switch up.png",
        "resources/rocker switch down.png",
        Button::TwoPositionsState::Off,
        bind (&RK05::wtprotSwitchClicked, this, _1),
        wtprotSwitchFrame);
}
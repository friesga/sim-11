#include "rk05.h"

void RK05::createBezel (Window* window, shared_ptr<RK05Config> rk05Config)
{
    Panel* panel = window->createPanel (rk05Config->cabinetPosition,
        RK05Config::unitHeight);
    panel->createFront ("resources/RK05-front.png", {0, 0, 1.0, 1.0});

    // PWR indicator, default on
    pwrIndicator_ = panel->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, pwrIndicatorFrame);

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

    // LOAD indicator, default off
    loadIndicator_ = panel->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, loadIndicatorFrame);

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
}
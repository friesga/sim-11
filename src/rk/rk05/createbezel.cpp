#include "rk05.h"

#include <functional>

using std::bind;
using std::placeholders::_1;
using std::to_string;

void RK05::createBezel (Window* window, const RK05Config& rk05Config)
{
    unique_ptr<PanelBuilder> panelBuilder =
        window->createFilePanelBuilder (rk05Config.cabinetPosition.value (),
            RK05Config::unitHeight);

    panelBuilder->createFront ("resources/RK05-front.png", {0, 0, 1.0, 1.0});

    // PWR indicator, default on
    pwrIndicator_ = panelBuilder->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::On, pwrIndicatorFrame);

    // RDY indicator, default off
    rdyIndicator_ = panelBuilder->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, readyIndicatorFrame);

    // ONCYL indicator, default off
    oncylIndicator_ = panelBuilder->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, oncylIndicatorFrame);

    // WTPROT indicator, default off
    wtprotIndicator_ = panelBuilder->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, wtprotIndicatorFrame);

    // LOAD indicator, default on
    loadIndicator_ = panelBuilder->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::On, loadIndicatorFrame);

    // WT indicator, default off
    wtIndicator_ = panelBuilder->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, wtIndicatorFrame);

    // RD indicator, default off
    rdIndicator_ = panelBuilder->createIndicator (
        "resources/white led off.png",
        "resources/white led on.png",
        Indicator::State::Off, rdIndicatorFrame);

    // FAULT indicator, default off
    faultIndicator_ = panelBuilder->createIndicator (
        "resources/FAULT off.png",
        "resources/FAULT on.png",
        Indicator::State::Off, faultIndicatorFrame);

    // RUN/LOAD switch, initial state up. The RUN/LOAD switch is a rocker
    // switch with two latched positions.
    runLoadSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"resources/rocker switch down.png",
        "resources/rocker switch up.png"},
        Button::TwoPositionsState::Down,
        bind (&RK05::runLoadSwitchClicked, this, _1),
        runLoadSwitchFrame);

    // WTPROT switch, initial state up. The WTPROT switch is a rocker switch,
    // spring loaded in the off position.
    wtprotSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"resources/rocker switch down.png",
        "resources/rocker switch up.png"},
        Button::MomentaryUpTwoPositionsState::Down,
        bind (&RK05::wtprotSwitchClicked, this, _1),
        wtprotSwitchFrame);

    panelBuilder->createFront ("resources/RK05 drive " +
        to_string (rk05Config.unitNumber) + ".png", numberLabelFrame);

    window->addPanel (panelBuilder->getPanel ());
}
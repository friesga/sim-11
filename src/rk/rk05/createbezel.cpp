#include "rk05.h"
#include "imagedata/openrasterfile/openrasterfile.h"

#include <functional>
#include <memory>

using std::bind;
using std::placeholders::_1;
using std::to_string;
using std::unique_ptr;
using std::make_unique;

void RK05::createBezel (Window* window, const RK05Config& rk05Config)
{
    unique_ptr<ImageContainer> imageContainer =
        make_unique<OpenRasterFile> ("resources/rk05.ora");

    unique_ptr<PanelBuilder> panelBuilder =
        window->createDataPanelBuilder (*imageContainer,
            rk05Config.cabinetPosition.value (), RK05Config::unitHeight);

    panelBuilder->createFront ("RK05 front");

    // PWR indicator, default on
    pwrIndicator_ = panelBuilder->createIndicator (
        "PWR off", "PWR on",
        Indicator::State::Off);

    // RDY indicator, default off
    rdyIndicator_ = panelBuilder->createIndicator (
        "RDY off", "RDY on",
        Indicator::State::Off);

    // ONCYL indicator, default off
    oncylIndicator_ = panelBuilder->createIndicator (
        "ONCYL off", "ONCYL on",
        Indicator::State::Off);

    // WTPROT indicator, default off
    wtprotIndicator_ = panelBuilder->createIndicator (
        "WTPROT off", "WTPROT on",
        Indicator::State::Off);

    // LOAD indicator, default on
    loadIndicator_ = panelBuilder->createIndicator (
        "LOAD off", "LOAD on",
        Indicator::State::Off);

    // WT indicator, default off
    wtIndicator_ = panelBuilder->createIndicator (
        "WT off", "WT on",
        Indicator::State::Off);

    // RD indicator, default off
    rdIndicator_ = panelBuilder->createIndicator (
        "RD off", "RD on",
        Indicator::State::Off);

    // FAULT indicator, default off
    faultIndicator_ = panelBuilder->createIndicator (
        "FAULT off", "FAULT on",
        Indicator::State::Off);

    // RUN/LOAD switch, initial state up. The RUN/LOAD switch is a rocker
    // switch with two latched positions.
    runLoadSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"switch LOAD", "switch RUN"},
        Button::TwoPositionsState::Down,
        bind (&RK05::runLoadSwitchClicked, this, _1));

    // WTPROT switch, initial state up. The WTPROT switch is a rocker switch,
    // spring loaded in the off position.
    wtprotSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"switch WTPROT off", "switch WTPROT on"},
        Button::MomentaryUpTwoPositionsState::Down,
        bind (&RK05::wtprotSwitchClicked, this, _1));

    panelBuilder->createFront ("drive " + to_string (rk05Config.unitNumber));

    window->addPanel (panelBuilder->getPanel ());
}
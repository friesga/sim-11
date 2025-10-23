#include "ky11_a.h"
#include "configdata/ka11/ky11_aconfig/ky11_aconfig.h"
#include "panel.h"
#include "imagedata/openrasterfile/openrasterfile.h"

#include <memory>
#include <functional>

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;

using std::make_unique;

KY11_A::KY11_A (Window* window, const KY11_AConfig& ky11_aConfig)
{
    createBezel (window, ky11_aConfig);
}

void KY11_A::createBezel (Window* window, const KY11_AConfig& ky11_aConfig)
{
    unique_ptr<ImageContainer> imageContainer =
        make_unique<OpenRasterFile> ("resources/pdp-11_20 front.ora");

    unique_ptr<PanelBuilder> panelBuilder =
        window->createDataPanelBuilder (*imageContainer,
            *ky11_aConfig.cabinetPosition, ky11_aConfig.unitHeight);    

    panelBuilder->createFront ("panel");

    powerSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"keyswitch off",
        "keyswitch power",
        "keyswitch lock",
        },
        Button::ThreePositionsState::Left,
        bind (&KY11_A::powerSwitchClicked, this, _1));

    runLight_ = panelBuilder->createIndicator ("run_off", "run_on",
        Indicator::State::On);

    createSwitchRegisterButtons (panelBuilder);


    window->addPanel (panelBuilder->getPanel ());
}

void KY11_A::createSwitchRegisterButtons (unique_ptr<PanelBuilder>& panelBuilder)
{
    // Create a compile-time loop to create the 16 switch register buttons
    // using a an Immediately Invoked Function Expression (IIFE) and a fold
    // expression. See e.g. 
    // https://www.fluentcpp.com/2021/03/05/stdindex_sequence-and-its-improvement-in-c20
    //
    [&] <size_t... I> (std::index_sequence<I...>)
    {
        (createSwitchRegisterButton<I> (panelBuilder), ...); 
    }
    (std::make_index_sequence<16> {});
}

void KY11_A::powerSwitchClicked (Button::State state)
{
}

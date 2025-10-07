#include "ba11_c.h"
#include "imagedata/openrasterfile/openrasterfile.h"

#include <memory>
#include <functional>

using std::unique_ptr;
using std::make_unique;
using std::bind;
using std::placeholders::_1;

//
// Support for the BA11-C Mounting Box
//
// Constructor
BA11_C::BA11_C (Bus* bus, Window* window, const BA11_CConfig& ba11cConfig)
    :
    bus_ {bus},
    frontWindow_ {window}
{
    createBezel (ba11cConfig.cabinetPosition.value ());
}

// Create the BA11-C panel at the specified position in the cabinet,
// and then start a loop handling the events and rendering lamps and
// switches.
//
// The KY11-A operator console contains a three-position rotary power switch
// with the following positions:
// OFF          Power is removed from the system,
// POWER        Power is applied to the system,
// PANEL LOCK   Power is applied to the system and the switches on the console
//              are non-operational.
// 
// ToDo: The image container to use should depend on the operator console
// assembled in the BA11-C.
//
void BA11_C::createBezel (Cabinet::Position cabinetPosition)
{
    unique_ptr<ImageContainer> imageContainer =
        make_unique<OpenRasterFile> ("resources/pdp-11_20 front.ora");

    unique_ptr<PanelBuilder> panelBuilder = 
       frontWindow_->createDataPanelBuilder (*imageContainer, cabinetPosition,
           BA11_CConfig::unitHeight);

    panelBuilder->createFront ("panel");

    powerSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"keyswitch off",
        "keyswitch power",
        "keyswitch lock",
         },
        Button::MomentaryThreePositionsState::Left,
        bind (&BA11_C::powerSwitchClicked, this, _1));
    
    frontWindow_->addPanel (panelBuilder->getPanel ());
}

void BA11_C::powerSwitchClicked (Button::State state)
{
}
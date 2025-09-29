#include "ba11_c.h"
#include "imagedata/openrasterfile/openrasterfile.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

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

// ToDo: The image container to use should depend on the operator console
// assembled in the BA11-C.
void BA11_C::createBezel (Cabinet::Position cabinetPosition)
{
    unique_ptr<ImageContainer> imageContainer =
        make_unique<OpenRasterFile> ("resources/pdp-11_20 front.ora");

    unique_ptr<PanelBuilder> panelBuilder = 
       frontWindow_->createDataPanelBuilder (*imageContainer, cabinetPosition,
           BA11_CConfig::unitHeight);

    panelBuilder->createFront ("panel");
    
    frontWindow_->addPanel (panelBuilder->getPanel ());
}
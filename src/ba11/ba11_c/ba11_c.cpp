#include "ba11_c.h"

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

void BA11_C::createBezel (Cabinet::Position cabinetPosition)
{
    unique_ptr<PanelBuilder> panelBuilder = 
       frontWindow_->createFilePanelBuilder (cabinetPosition, BA11_CConfig::unitHeight);
    
    frontWindow_->addPanel (panelBuilder->getPanel ());
}
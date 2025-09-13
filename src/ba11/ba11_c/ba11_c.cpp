#include "ba11_c.h"

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
    Panel* panel = 
        frontWindow_->createPanel (cabinetPosition, BA11_CConfig::unitHeight);
}
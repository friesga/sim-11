#include "ba11cconfig.h"

const RackUnit BA11_CConfig::unitHeight {6_ru};

BA11_CConfig::BA11_CConfig (Cabinet::Position cabinetPosition)
{
    this->cabinetPosition = cabinetPosition;
}
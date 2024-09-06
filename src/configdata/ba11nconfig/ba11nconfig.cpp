#include "ba11nconfig.h"

const RackUnit BA11_NConfig::unitHeight {3_ru};

BA11_NConfig::BA11_NConfig (Cabinet::Position cabinetPosition)
{
    this->cabinetPosition = make_shared<Cabinet::Position> (cabinetPosition);
}
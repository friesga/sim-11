#include "ba11lconfig.h"

const RackUnit BA11_LConfig::unitHeight {3_ru};

BA11_LConfig::BA11_LConfig (Cabinet::Position cabinetPosition)
{
    this->cabinetPosition = make_shared<Cabinet::Position> (cabinetPosition);
}
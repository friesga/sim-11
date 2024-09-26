#include "rl01_02.h"

void RL01_02::createBezel (Window* window, shared_ptr<Cabinet::Position> cabinetPosition)
{
    Panel* panel = window->createPanel (cabinetPosition,
        RLUnitConfig::unitHeight);
    panel->createFront ("../../assets/RL02-front.png", {0, 0, 1.0, 1.0});
}
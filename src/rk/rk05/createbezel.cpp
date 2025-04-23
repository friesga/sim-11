#include "rk05.h"

void RK05::createBezel (Window* window, shared_ptr<RK05Config> rk05Config)
{
    Panel* panel = window->createPanel (rk05Config->cabinetPosition,
        RK05Config::unitHeight);
    panel->createFront ("resources/RK05-front.png", {0, 0, 1.0, 1.0});
}
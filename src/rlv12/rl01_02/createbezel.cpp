#include "rl01_02.h"

#include <functional>

using std::bind;
using std::placeholders::_1;
using std::to_string;

void RL01_02::createBezel (Window* window,
    shared_ptr<RLUnitConfig> rlUnitConfig)
{
    Panel* panel = window->createPanel (rlUnitConfig->cabinetPosition,
        RLUnitConfig::unitHeight);
    panel->createFront ("../../assets/RL02-front.png", {0, 0, 1.0, 1.0});

    // LOAD IndicatorButton
    loadButton_ = panel->createSDLIndicatorLatchingButton ({
        "../../assets/Load_up_off.png",
        "../../assets/Load_up_on.png",
        "../../assets/Load_down_off.png",
        "../../assets/Load_down_on.png"},
        Button::State::Up, bind (&RL01_02::loadButtonClicked, this, _1),
        Indicator::State::On, loadButtonFrame);

    // READY indicator, default off
    readyIndicator_ = panel->createIndicator (
        "../../assets/ready_" + to_string (rlUnitConfig->unitNumber) + "_off.png",
        "../../assets/ready_" + to_string (rlUnitConfig->unitNumber) + "_on.png",
        Indicator::State::Off, readyIndicatorFrame);
}

void RL01_02::loadButtonClicked (Button::State state)
{
    if (state == Button::State::Down)
        sendTrigger (SpinUp {});
    else
        sendTrigger (SpinDown {});
}
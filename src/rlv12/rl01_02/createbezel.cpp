#include "rl01_02.h"
#include "rlv12/rlv12const.h"

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

    // FAULT indicator, default off
    faultIndicator_ = panel->createIndicator (
        "../../assets/fault_off.png",
        "../../assets/fault_on.png",
        Indicator::State::Off, faultIndicatorFrame);

    // WRITE PROTECT switch, initial state depends on unit configuration
    writeProtectButton_ = panel->createLatchingButton (
        "../../assets/write_protect_on.png",
        "../../assets/write_protect_off.png",
        rlUnitConfig->writeProtect ? Button::State::Down : Button::State::Up,
        bind (&RL01_02::writeProtectButtonClicked, this, _1),
        writeProtectButtonFrame);
}

void RL01_02::loadButtonClicked (Button::State state)
{
    if (state == Button::State::Down)
        sendTrigger (SpinUp {});
    else
        sendTrigger (SpinDown {});
}

void RL01_02::writeProtectButtonClicked (Button::State state)
{
    if (state == Button::State::Down)
    {
        unitStatus_ |= Bitmask (Status::WRITE_PROTECT);
        driveStatus_ |= RLV12const::MPR_GS_WriteLock;
    }
    else
    {
        unitStatus_ &= ~Bitmask (Status::WRITE_PROTECT);
        driveStatus_ &= ~RLV12const::MPR_GS_WriteLock;
    }
}
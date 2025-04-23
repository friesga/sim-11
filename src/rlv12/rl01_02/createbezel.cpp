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
    panel->createFront ("resources/RL02-front.png", {0, 0, 1.0, 1.0});

    // LOAD IndicatorButton
    loadButton_ = panel->createSDLIndicatorLatchingButton ({
        "resources/Load_up_off.png",
        "resources/Load_up_on.png",
        "resources/Load_down_off.png",
        "resources/Load_down_on.png"},
        Button::TwoPositionsState::Up, bind (&RL01_02::loadButtonClicked, this, _1),
        Indicator::State::On, loadButtonFrame);

    // READY indicator, default off
    readyIndicator_ = panel->createIndicator (
        "resources/ready_" + to_string (rlUnitConfig->unitNumber) + "_off.png",
        "resources/ready_" + to_string (rlUnitConfig->unitNumber) + "_on.png",
        Indicator::State::Off, readyIndicatorFrame);

    // FAULT indicator, default off
    faultIndicator_ = panel->createIndicator (
        "resources/fault_off.png",
        "resources/fault_on.png",
        Indicator::State::Off, faultIndicatorFrame);

    // WRITE PROTECT switch, initial state depends on unit configuration
    writeProtectButton_ = panel->createLatchingButton (
        "resources/write_protect_on.png",
        "resources/write_protect_off.png",
        rlUnitConfig->writeProtect ? Button::TwoPositionsState::Down : Button::TwoPositionsState::Up,
        bind (&RL01_02::writeProtectButtonClicked, this, _1),
        writeProtectButtonFrame);
}

void RL01_02::loadButtonClicked (Button::State state)
{
    if (get<Button::TwoPositionsState> (state) == Button::TwoPositionsState::Down)
        sendTrigger (SpinUp {});
    else
        sendTrigger (SpinDown {});
}

void RL01_02::writeProtectButtonClicked (Button::State state)
{
    if (get<Button::TwoPositionsState> (state) == Button::TwoPositionsState::Down)
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
#include "rl01_02.h"
#include "rlv12/rlv12const.h"

#include <functional>

using std::bind;
using std::placeholders::_1;
using std::to_string;

void RL01_02::createBezel (Window* window,
    const RLUnitConfig& rlUnitConfig)
{
    unique_ptr<PanelBuilder> panelBuilder =
        window->createFilePanelBuilder (rlUnitConfig.cabinetPosition.value (),
            RLUnitConfig::unitHeight);

    panelBuilder->createFront ("resources/RL02-front.png", {0, 0, 1.0, 1.0});

    // LOAD IndicatorButton
    loadButton_ = panelBuilder->createIndicatorLatchingButton ({
        "resources/Load_up_off.png",
        "resources/Load_up_on.png",
        "resources/Load_down_off.png",
        "resources/Load_down_on.png"},
        Button::TwoPositionsState::Up, bind (&RL01_02::loadButtonClicked, this, _1),
        Indicator::State::On, loadButtonFrame);

    // READY indicator, default off
    readyIndicator_ = panelBuilder->createIndicator (
        "resources/ready_" + to_string (rlUnitConfig.unitNumber) + "_off.png",
        "resources/ready_" + to_string (rlUnitConfig.unitNumber) + "_on.png",
        Indicator::State::Off, readyIndicatorFrame);

    // FAULT indicator, default off
    faultIndicator_ = panelBuilder->createIndicator (
        "resources/fault_off.png",
        "resources/fault_on.png",
        Indicator::State::Off, faultIndicatorFrame);

    // WRITE PROTECT switch, initial state depends on unit configuration
    writeProtectButton_ = panelBuilder->createMultiPositionSwitch (
        {"resources/write_protect_on.png",
        "resources/write_protect_off.png"},
        rlUnitConfig.writeProtect ? 
            Button::CenteredTwoPositionsState::Down : 
            Button::CenteredTwoPositionsState::Up,
        bind (&RL01_02::writeProtectButtonClicked, this, _1),
        writeProtectButtonFrame);

    window->addPanel (panelBuilder->getPanel ());
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
    if (get<Button::CenteredTwoPositionsState> (state) == 
        Button::CenteredTwoPositionsState::Down)
    {
        setWriteProtected (true);
        driveStatus_ |= RLV12const::MPR_GS_WriteLock;
    }
    else
    {
        setWriteProtected (false);
        driveStatus_ &= ~RLV12const::MPR_GS_WriteLock;
    }
}
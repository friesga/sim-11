#include "rl01_02.h"
#include "rlv12/rlv12const.h"
#include "imagedata/openrasterfile/openrasterfile.h"

#include <functional>
#include <memory>

using std::bind;
using std::placeholders::_1;
using std::to_string;
using std::unique_ptr;
using std::make_unique;

void RL01_02::createBezel (Window* window,
    const RLUnitConfig& rlUnitConfig)
{
    unique_ptr<ImageContainer> imageContainer =
        make_unique<OpenRasterFile> ("resources/rl01_02.ora");

    unique_ptr<PanelBuilder> panelBuilder =
        window->createDataPanelBuilder (*imageContainer,
            rlUnitConfig.cabinetPosition.value (), RLUnitConfig::unitHeight);

    panelBuilder->createFront ("RL01_front");

    // LOAD IndicatorButton
    loadButton_ = panelBuilder->createIndicatorLatchingButton ({
        "load_up_off", "load_up_on",
        "load_down_off", "load_down_on"},
        Button::TwoPositionsState::Up, bind (&RL01_02::loadButtonClicked, this, _1),
        Indicator::State::On, loadButtonFrame);

    // READY indicator, default off
    readyIndicator_ = panelBuilder->createIndicator (
        "ready" + to_string (rlUnitConfig.unitNumber) + "_off",
        "ready" + to_string (rlUnitConfig.unitNumber) + "_on",
        Indicator::State::Off, readyIndicatorFrame);

    // FAULT indicator, default off
    faultIndicator_ = panelBuilder->createIndicator ("fault_off", "fault_on",
        Indicator::State::Off, faultIndicatorFrame);

    // WRITE PROTECT switch, initial state depends on unit configuration
    writeProtectButton_ = panelBuilder->createMultiPositionSwitch (
        {"write_prot_on", "write_prot_off"},
        rlUnitConfig.writeProtect ? 
            Button::CenteredTwoPositionsState::Down : 
            Button::CenteredTwoPositionsState::Up,
        bind (&RL01_02::writeProtectButtonClicked, this, _1),
        writeProtectButtonFrame);

    if (driveType_ == DriveType::RL02)
        panelBuilder->createFront ("RL02_label");

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
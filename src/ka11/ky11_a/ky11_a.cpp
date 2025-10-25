#include "ky11_a.h"
#include "configdata/ka11/ky11_aconfig/ky11_aconfig.h"
#include "panel.h"
#include "imagedata/openrasterfile/openrasterfile.h"

#include <memory>
#include <functional>

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;

using std::make_unique;

KY11_A::KY11_A (Bus* bus, Window* window, const KY11_AConfig& ky11_aConfig)
    :
    bus_ {bus}
{
    stateMachine_ = make_unique<StateMachine> (this);

    unique_ptr<ImageContainer> imageContainer =
        make_unique<OpenRasterFile> ("resources/pdp-11_20 front.ora");

    unique_ptr<PanelBuilder> panelBuilder =
        window->createDataPanelBuilder (*imageContainer,
            *ky11_aConfig.cabinetPosition, ky11_aConfig.unitHeight);

    createBezel (window, ky11_aConfig, panelBuilder);

    switchRegister_ = make_unique<SwitchRegister> (panelBuilder);
    addressRegister_ = make_unique<AddressRegister> (panelBuilder);
    dataRegister_ = make_unique<DataRegister> (panelBuilder);

    window->addPanel (panelBuilder->getPanel ());
}

void KY11_A::createBezel (Window* window, const KY11_AConfig& ky11_aConfig,
    unique_ptr<PanelBuilder>& panelBuilder)
{
    panelBuilder->createFront ("panel");

    runLight_ = panelBuilder->createIndicator ("run_off", "run_on",
        Indicator::State::On);

    powerSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"keyswitch off",
        "keyswitch power",
        "keyswitch lock",
        },
        Button::ThreePositionsState::Left,
        bind (&KY11_A::powerSwitchClicked, this, _1));

    loadAddressSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"load_addr_up", "load_addr_down"},
        Button::MomentaryDownTwoPositionsState::Up,
        bind (&KY11_A::loadAddressClicked, this, _1));

    loadAddressSwitch_ = panelBuilder->createMultiPositionSwitch (
        {"exam_up", "exam_down"},
        Button::MomentaryDownTwoPositionsState::Up,
        bind (&KY11_A::loadAddressClicked, this, _1));
}


void KY11_A::powerSwitchClicked (Button::State state)
{
}

// The LOAD ADDR switch transfers the switch register contents to the Bus
// Address Register (BAR) [...]. This bus address, displayed in the ADDRESS
// REGISTER, provides an address for the console functions of EXAM, DEP and
// START. (DEC-11-HR1B-D Table 3-2)
//
void KY11_A::loadAddressClicked (Button::State state)
{
    stateMachine_->dispatch (LOAD_ADDR_Pressed {});
}

// The EXAM switch transfers the contents of the bus address (which is
// specified by the Bus Address Register) for DATA display. After use, the
// data appear on the DATA display and the address of the data is in the
// ADDRESS REGISTER.
// 
// A LOAD ADDR operation pre-establishes the initial address; sequential
// addresses occur automatically.
// 
// If the EXAM switch is depressed twice in
// succession, the contents of the next sequential bus address are displayed
// in DATA. This action is repeated each time EXAM is depressed provided no
// other switch is used between these steps.
//
// Whenever LOAD ADDR or DEP switch is used, it destroys the incrementing
// sequence.The next time EXAM is used, it displays the current Bus Address
// Register address rather than the next sequential address.
//
// Source: DEC-11-HR1B-D, Table 3-2.
//
void KY11_A::examClicked (Button::State state)
{
    stateMachine_->dispatch (EXAM_Pressed {});
}

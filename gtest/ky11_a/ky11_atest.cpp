#include "proc/ka11/ky11_a/ky11_a.h"
#include "bus/unibus/unibus.h"
#include "proc/ka11/ka11cpudata/ka11cpudata.h"
#include "proc/common/pseudoMMU/pseudommu.h"
#include "proc/common/composite_cpucontroller/composite_cpucontroller.h"
#include "proc/ka11/executor/executor.h"
#include "proc/ka11/calculator/calculator.h"
#include "proc/common/pseudo_haltmode/pseudo_haltmode.h"
#include "proc/common/basicprocessorexceptionhandler/basicprocessorexceptionhandler.h"
#include "../fakesdl/fakewindow/fakewindow.h"

#include <gtest/gtest.h>

TEST(KY11_A, ky11_a)
{
    Unibus bus;
    KA11CpuData cpuData {};
    PseudoMMU mmu {&bus, &cpuData};
    CompositeCpuController<KA11_Executor, KA11Calculator,
        PseudoHaltMode, BasicProcessorExceptionHandler> cpuController {&bus, &cpuData, &mmu};
    FakeWindow window {};

    KY11_A ky11a {&bus, &cpuController, &window, KY11_AConfig {Cabinet::Position {0,0}}};

    // Switch power on
    ky11a.enableHaltSwitchClicked (Button::State {Button::TwoPositionsState::Down});
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    ky11a.setSwitchRegister (0173100);
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    KY11_A::KY11_AStatus status = ky11a.getKY11_AStatus ();
    EXPECT_EQ (status.addressIndicatorsValues, 0173100);
} 
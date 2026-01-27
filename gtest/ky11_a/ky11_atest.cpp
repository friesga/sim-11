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

// Definition of a test fixture
class KY11_ATest : public testing::Test
{
protected:
    Unibus bus;
    KA11CpuData cpuData {};
    PseudoMMU mmu {&bus, &cpuData};
    CompositeCpuController<KA11_Executor, KA11Calculator,
        PseudoHaltMode, BasicProcessorExceptionHandler> cpuController {&bus, &cpuData, &mmu};
    FakeWindow window {};

    KY11_A ky11a {&bus, &cpuController, &window, KY11_AConfig {Cabinet::Position {0,0}}};

    KY11_ATest ()
    {
        ky11a.enableHaltSwitchClicked (Button::State {Button::TwoPositionsState::Down});
    }
};

// Test sequence:
// ENABLE/HALT -> HALT
// POWER -> POWER
//
// Expected result:
// ADDRESS REGISTER contains 0
// DATA REGISTER contains 0
TEST_F (KY11_ATest, initialState)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});

    EXPECT_EQ (*ky11a.addressRegister_, 0);
    EXPECT_EQ (*ky11a.dataRegister_, 0);
}

// Test sequence:
// ENABLE/HALT -> HALT
// POWER -> POWER
// Set SWITCH REGISTER to 0173100
// LOAD ADDR pushed
// 
// Expected result:
// ADDRESS REGISTER contains 0173100
//
TEST_F (KY11_ATest, addressCanBeLoaded)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0173100;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 0173100);
}
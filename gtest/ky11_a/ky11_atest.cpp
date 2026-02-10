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
#include "ms11p/ms11p.h"
#include "proc/common/datapaths/datapaths.h"

#include <gtest/gtest.h>

// Definition of a test fixture
class KY11_ATest : public testing::Test
{
protected:
    Unibus bus;
    MS11P ms11p {&bus};
    KA11CpuData cpuData {};
    PseudoMMU mmu {&bus, &cpuData};
    DataPaths dataPaths_ {&bus, &mmu};
    CompositeCpuController<KA11_Executor, KA11Calculator,
        PseudoHaltMode, BasicProcessorExceptionHandler> cpuController {&bus, &cpuData, &dataPaths_};
    FakeWindow window {};

    KY11_A ky11a {&bus, cpuController, &window, KY11_AConfig {Cabinet::Position {0,0}}};
    KA11MachineState machineState {&bus, &cpuData, cpuController, &dataPaths_, ky11a};

    KY11_ATest ()
    {
        bus.installModule (&ms11p);

        for (size_t address = 0; address < 100; address += 2)
            ms11p.writeWord (BusAddress (address, BusAddress::Width::_16Bit),
                01000 + address);

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
// POWER on
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

TEST_F (KY11_ATest, addressCanBeExamined)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 0);
    EXPECT_EQ (*ky11a.dataRegister_, 01000);
}

TEST_F (KY11_ATest, addressSequenceCanBeExamined)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    // Start Examine sequence
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 2);
    EXPECT_EQ (*ky11a.dataRegister_, 01002);
}

TEST_F (KY11_ATest, loadAddressResetsExamineSequence)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 0);
}

TEST_F (KY11_ATest, dataCanBeDeposited)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    *ky11a.switchRegister_ = 01000;
    ky11a.depSwitchClicked (Button::State {Button::MomentaryUpTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 0);
    EXPECT_EQ (*ky11a.dataRegister_, 01000);
}

TEST_F (KY11_ATest, dataSequenceCanBeDeposited)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    *ky11a.switchRegister_ = 01000;
    ky11a.depSwitchClicked (Button::State {Button::MomentaryUpTwoPositionsState::Down});
    *ky11a.switchRegister_ = 01002;
    ky11a.depSwitchClicked (Button::State {Button::MomentaryUpTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 2);
    EXPECT_EQ (*ky11a.dataRegister_, 01002);
}

TEST_F (KY11_ATest, loadAddressResetsDepositSequence)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    *ky11a.switchRegister_ = 01000;
    ky11a.depSwitchClicked (Button::State {Button::MomentaryUpTwoPositionsState::Down});
    *ky11a.switchRegister_ = 01002;
    ky11a.depSwitchClicked (Button::State {Button::MomentaryUpTwoPositionsState::Down});

    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 0);
    EXPECT_EQ (*ky11a.dataRegister_, 01002);
}

TEST_F (KY11_ATest, examResetsDepositSequence)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    *ky11a.switchRegister_ = 01000;
    ky11a.depSwitchClicked (Button::State {Button::MomentaryUpTwoPositionsState::Down});
    *ky11a.switchRegister_ = 01002;
    ky11a.depSwitchClicked (Button::State {Button::MomentaryUpTwoPositionsState::Down});

    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 2);
    EXPECT_EQ (*ky11a.dataRegister_, 01002);

    // Verify we're actually in the ExamineSequence state by examining another word
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 4);
    EXPECT_EQ (*ky11a.dataRegister_, 01004);
}

TEST_F (KY11_ATest, depResetsExamineSequence)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    *ky11a.switchRegister_ = 01002;
    ky11a.depSwitchClicked (Button::State {Button::MomentaryUpTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 2);
    EXPECT_EQ (*ky11a.dataRegister_, 01002);

    // Verify we're actually in the DepositSequence state by depositing another word
    *ky11a.switchRegister_ = 01004;
    ky11a.depSwitchClicked (Button::State {Button::MomentaryUpTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 4);
    EXPECT_EQ (*ky11a.dataRegister_, 01004);
}

TEST_F (KY11_ATest, startWhileEnabled)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.enableHaltSwitchClicked (Button::State {Button::TwoPositionsState::Up});

    EXPECT_EQ (ky11a.runLight_->indicatorState (), Indicator::State::Off);
    ky11a.startSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    EXPECT_EQ (ky11a.runLight_->indicatorState (), Indicator::State::On);
}

TEST_F (KY11_ATest, startResetsExamineSequence)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.startSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    EXPECT_EQ (*ky11a.addressRegister_, 2);
    ky11a.startSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    EXPECT_EQ (*ky11a.addressRegister_, 2);

    // Verify we're actually in the AddressLoaded state, so the first EXAM opens the 
    // current address again
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    EXPECT_EQ (*ky11a.addressRegister_, 2);
    EXPECT_EQ (*ky11a.dataRegister_, 01002);
}

TEST_F (KY11_ATest, startResetsDepositSequence)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    *ky11a.switchRegister_ = 01000;
    ky11a.depSwitchClicked (Button::State {Button::MomentaryUpTwoPositionsState::Down});
    *ky11a.switchRegister_ = 01002;
    ky11a.depSwitchClicked (Button::State {Button::MomentaryUpTwoPositionsState::Down});

    ky11a.startSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    // Verify we're actually in the AddressLoaded state, so the first EXAM opens the 
    // current address again
    ky11a.examSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    EXPECT_EQ (*ky11a.addressRegister_, 2);
    EXPECT_EQ (*ky11a.dataRegister_, 01002);
}

TEST_F (KY11_ATest, haltHaltsProgramOperation)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});
    *ky11a.switchRegister_ = 0;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});
    ky11a.enableHaltSwitchClicked (Button::State {Button::TwoPositionsState::Up});

    EXPECT_EQ (ky11a.runLight_->indicatorState (), Indicator::State::Off);
    ky11a.startSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    EXPECT_EQ (ky11a.runLight_->indicatorState (), Indicator::State::On);

    ky11a.enableHaltSwitchClicked (Button::State {Button::TwoPositionsState::Down});
    EXPECT_EQ (ky11a.runLight_->indicatorState (), Indicator::State::Off);
}

TEST_F (KY11_ATest, resetLoadsTempRegister)
{
    ky11a.powerSwitchClicked (Button::State {Button::ThreePositionsState::Center});

    // Load BAR and TEMP registers
    *ky11a.switchRegister_ = 01000;
    ky11a.loadAddressSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    // Force the contents of the Address Register to 0 to be able to verify its
    // contents are modified by the reset operation.
    *ky11a.addressRegister_ = 0;
    ky11a.startSwitchClicked (Button::State {Button::MomentaryDownTwoPositionsState::Down});

    // The Address Register now should contain the contents of the TEMP register, i.e. the
    // result of the previous LOAD ADDR operation.
    EXPECT_EQ (*ky11a.addressRegister_, 01000);
}
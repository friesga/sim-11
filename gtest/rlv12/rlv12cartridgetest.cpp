#include "pdp11/pdp11.h"
#include "disk/rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <gtest/gtest.h>
#include <chrono>

using namespace std::chrono_literals;

// Definition of the test fixture
class RLV12CartridgeTest : public ::testing::Test
{
protected:
    // Use our own set of definitions to avoid common cause issues
    static constexpr u16 RLBASE = 0174400;
    static constexpr u16 RLCSR = RLBASE;
    static constexpr u16 RLBAR = RLBASE + 02;
    static constexpr u16 RLDAR = RLBASE + 04;
    static constexpr u16 RLMPR = RLBASE + 06;
    // A 16 bit system has no BAE register

    // CSR bit definitions
    static constexpr u16  CSR_GetStatusCommand    = (2 << 1);
    static constexpr u16  CSR_ReadDataCommand     = (06 << 1);
    static constexpr u16  CSR_DriveReady          = (1 << 0);
    static constexpr u16  CSR_ControllerReady     = (1 << 7);
    static constexpr u16  CSR_OperationIncomplete = (1 << 10);
    static constexpr u16  CSR_DriveError          = (1 << 14);
    static constexpr u16  CSR_CompositeError      = (1 << 15);
    static constexpr u16  CSR_Drive0              = 0;
    static constexpr u16  CSR_Drive1              = (1 << 8);
    static constexpr u16  CSR_Drive2              = (2 << 8);
    static constexpr u16  CSR_Drive3              = (3 << 8);

    // DAR bit definitions
    static constexpr u16 DAR_Marker               = (1 << 0);
    static constexpr u16 DAR_GetStatus            = (1 << 1);
    static constexpr u16 DAR_Reset                = (1 << 3);

    // MPR bit definitions
    static constexpr u16 MPR_BrushHome            = (1 << 3);
    static constexpr u16 MPR_HeadsOut             = (1 << 4);
    static constexpr u16 MPR_CoverOpen            = (1 << 5);
    static constexpr u16 MPR_DriveType            = (1 << 7);
    static constexpr u16 MPR_DriveSelectError     = (1 << 8);
    static constexpr u16 MPR_VolumeCheck          = (1 << 9);
    static constexpr u16 MPR_WriteLock            = (1 << 13);
    static const u16 MPR_LockOn                   = 5;


    // Create bus structure, an RLV12 device and install the device
    Qbus bus;
    MSV11D* msv11 = new MSV11D (&bus);
    RLV12* rlv12Device = new RLV12 (&bus);

    void SetUp() override
    {
        // Create a minimal system conisting of just the bus
        // and the RLV12 device to be tested.
        bus.installModule (msv11);bus.installModule (rlv12Device);
    }

    void waitForControllerReady ()
    {
        u16 result;
        do
        {
            SimulatorClock::forwardClock (100ms);
            result = rlv12Device->read (RLCSR);
        }
        while (!(result & CSR_ControllerReady));
    }
};

TEST_F (RLV12CartridgeTest, driveReportsCoverOpen)
{
    RLUnitConfig rlUnitConfig
    ({
        .unitType = RLUnitConfig::RLUnitType::RL01
    });

    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device->unit (0)->init (RLUnitConfig {rlUnitConfig}),
        StatusCode::Success);

    // Verify the controller is ready to perform an operation but the drive
    // is not ready.
    waitForControllerReady ();

    // Both the CSR and the DAR must be programmed to perform a Get Status
    // command. The DAR must be programmed with the Get Status bit. Then
    // a Get Status command can be placed in the CSR.
    rlv12Device->writeWord (RLDAR, DAR_GetStatus);

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrups disabled and a Get Status Command (02)
    // in the function bits.
    rlv12Device->writeWord (RLCSR, CSR_GetStatusCommand);

    waitForControllerReady ();

    // Expected result in the MPR register: Load State, Brush Home, Cover open,
    // Drive Type RL01.
    u16 mpr = rlv12Device->read (RLMPR);
    ASSERT_EQ (mpr, MPR_BrushHome | MPR_CoverOpen);
}
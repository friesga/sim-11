#include "pdp11/pdp11.h"
#include "msv11d/msv11d.h"
#include "rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <gtest/gtest.h>
#include <memory>
#include <chrono>

using std::make_shared;

using namespace std::chrono_literals;

// Definition of the test fixture
class RLV12ReadHeaderTest : public ::testing::Test
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
    static constexpr u16  CSR_DriveReady          = 1;
    static constexpr u16  CSR_GetStatusCommand    = (02 << 1);
    static constexpr u16  CSR_SeekCommand         = (03 << 1);
    static constexpr u16  CSR_ReadHeaderCommand   = (04 << 1);
    static constexpr u16  CSR_WriteDataCommand    = (05 << 1);
    static constexpr u16  CSR_ReadDataCommand     = (06 << 1);
    static constexpr u16  CSR_ControllerReady     = (1 << 7);
    static constexpr u16  CSR_OperationIncomplete = (1 << 10); 
    static constexpr u16  CSR_CompositeError      = (1 << 15); 
    static constexpr u16  CSR_Drive0              = (0 << 8);
    static constexpr u16  CSR_Drive1              = (1 << 8);
    static constexpr u16  CSR_Drive2              = (2 << 8);
    static constexpr u16  CSR_Drive3              = (3 << 8);

    // DAR bit definitions
    static constexpr u16 DAR_Marker    = (1 << 00);

    // DAR bit definitions for a Get Status Command
    static constexpr u16 DAR_GetStatus = (1 << 01);
    static constexpr u16 DAR_Reset     = (1 << 03);

    // DAR bit definitions for a Seek Command
    static constexpr u16 DAR_Seek          = (0 << 01);
    static constexpr u16 DAR_DirectionOut  = (1 << 02);
    static constexpr u16 DAR_HeadSelect    = (1 << 04);
    inline u16 DAR_cylinderDifference (u16 x) { return x << 07; }

    // MPR bit definitions for a Seek Command
    inline u16 MPR_cylinder (u16 x) { return x >> 07; }

    // Create bus structure, an RLV12 device and install the device
    Qbus bus;
    MSV11D *msv11 = new MSV11D (&bus);
    RLV12 *rlv12Device = new RLV12 (&bus);

    void waitForControllerReady ()
    {
        u16 result;
        do
        {
            SimulatorClock::forwardClock (100ms);
            rlv12Device->read (RLCSR, &result);
        }
        while (!(result & CSR_ControllerReady));
    }

    void waitForDriveReady ()
    {
        u16 result;
        do
        {
            SimulatorClock::forwardClock (100ms);
            rlv12Device->read (RLCSR, &result);
        }
        while ((result & (CSR_ControllerReady | CSR_DriveReady)) !=
            (CSR_ControllerReady | CSR_DriveReady));
    }


    void SetUp() override
    {
        // Create a minimal system, conisting of just the bus, memory
        // and the RLV12 device to be tested.
        bus.installModule (msv11);
        bus.installModule (rlv12Device);

        // This unit configuration uses the default spin_up_time of zero seconds
        // so the drive immediately locks on to cylinder 0.
        RLUnitConfig rlUnitConfig
        ({
            .fileName = "rl01.dsk",
            .newFile = true,
            .overwrite = true
        });

        // Make sure the controller has started
        waitForControllerReady ();

        // Attach a new disk to unit 0
        ASSERT_EQ (rlv12Device->unit (0)->init (make_shared<RLUnitConfig> (rlUnitConfig)), 
            StatusCode::Success);

        // Clear errors and volume check condition
        rlv12Device->writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);
        rlv12Device->writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

        waitForControllerReady ();
    }
};

// Verify the correct execution of a Read Header command. This command reads
// the first header encountered on the current track of the selected drive.
TEST_F (RLV12ReadHeaderTest, readHeaderSucceeds)
{
    // Verify the controller is ready to perform an operation (the drive
    // does not have to be ready)
    u16 result;
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result & CSR_ControllerReady, CSR_ControllerReady);

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Read Header Command (04)
    // in the function bits.
    rlv12Device->writeWord (RLCSR, CSR_ReadHeaderCommand);

    waitForControllerReady ();

    // Verify the controller is ready
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result, CSR_ControllerReady | CSR_ReadHeaderCommand | 
        CSR_DriveReady);

    // Expected result in the MPR register: Sector Address, Head Select and
    // Cylinder Address. Sequential rotation has taken place.
    u16 mpr;
    rlv12Device->read (RLMPR, &mpr);
    ASSERT_EQ (mpr, 01);

    // The second MPR read should deliver all zero's
    rlv12Device->read (RLMPR, &mpr);
    ASSERT_EQ (mpr, 00);

    // The third read should deliver the CRC of the header
    // We presume the calculation is correct
    rlv12Device->read (RLMPR, &mpr);
    ASSERT_EQ (mpr, 64513);
}

// Verify a Read Header Command
TEST_F (RLV12ReadHeaderTest, headerHasCorrectContents)
{
    // Verify both controller and drive are ready
    u16 result;
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady | CSR_DriveReady);

    // Seek to a cylinder to be able to verify the correct contents of
    //the header
    rlv12Device->writeWord (RLDAR, 
        DAR_Marker | DAR_Seek | DAR_DirectionOut | DAR_cylinderDifference (10));
    rlv12Device->writeWord (RLCSR, CSR_SeekCommand);

    // Wait for the seek to be completed
    SimulatorClock::forwardClock (500ms);
    waitForDriveReady ();

    // Verify both controller and drive are ready and no error is
    // indicated
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // Execute a Read Header command
    rlv12Device->writeWord (RLCSR, CSR_ReadHeaderCommand);

    // Wait for command completion
    SimulatorClock::forwardClock (500ms);
    waitForDriveReady ();

    // Verify both controller and drive are ready again and no error is
    // indicated
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // Expected result in the MPR register: Sector Address, Head Select and
    // Cylinder Address
    u16 mpr;
    rlv12Device->read (RLMPR, &mpr);
    ASSERT_EQ (MPR_cylinder (mpr), 10);
}

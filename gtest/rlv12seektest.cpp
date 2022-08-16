#include "lsi11/lsi11.h"
#include "msv11d/msv11d.h"
#include "rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <gtest/gtest.h>

// Write to unit tests.

// Definition of the test fixture
class RLV12SeekTest : public ::testing::Test
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
    QBUS bus;
    MSV11D msv11;
    RLV12 rlv12Device {};

    void SetUp() override
    {
        // Create a minimal system, conisting of just the bus, memory
        // and the RLV12 device to be tested.
        bus.installModule (1, &msv11);
        bus.installModule (2, &rlv12Device);

        char const *argvSet0[] =
        {
            "sim-11",
            "-q"    // quiet
        };
        CmdLineOptions::processOptions (sizeof (argvSet0) /sizeof (argvSet0[0]),
            argvSet0);
    }
};

// Verify the correct execution of a Seek command
TEST_F (RLV12SeekTest, seekSucceeds)
{
    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device.unit (0)->attach ("rl01.dsk",
            Bitmask(AttachFlags::NewFile | AttachFlags::Overwrite)), 
        StatusCode::OK);

    // Clear errors and volume check condition
    rlv12Device.writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);
    rlv12Device.writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

    // Verify the controller is ready to perform an operation (the drive
    // does not have to be ready)
    u16 result;
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & CSR_ControllerReady, CSR_ControllerReady);

    // Load DAR with seek parameters
    rlv12Device.writeWord (RLDAR, DAR_Marker | DAR_Seek | DAR_DirectionOut |
        DAR_cylinderDifference (5));

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Seek Command (03)
    // in the function bits.
    rlv12Device.writeWord (RLCSR, CSR_SeekCommand);

    // Verify the controller is ready to accept a command for another unit
    // while the drive is not ready
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady);

    // Wait for command completion
    std::this_thread::sleep_for (std::chrono::milliseconds (500));

    // Verify now both controller and drive are ready
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady | CSR_DriveReady);

    // Verify the cylinder position
        // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Read Header Command (04)
    // in the function bits.
    rlv12Device.writeWord (RLCSR, CSR_ReadHeaderCommand);

    // Wait for command completion
    std::this_thread::sleep_for (std::chrono::milliseconds (500));

    // Verify the controller is ready
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result, CSR_ControllerReady | CSR_ReadHeaderCommand | 
        CSR_DriveReady);

    // Expected result in the MPR register: Sector Address, Head Select and
    // Cylinder Address
    u16 mpr;
    rlv12Device.read (RLMPR, &mpr);
    ASSERT_EQ (MPR_cylinder (mpr), 5);
}

// Verify a seek command for a different unit can be performed during
// execution of the first command
TEST_F (RLV12SeekTest, parallelSeeksSucceed)
{
    // Attach a new disk to unit 0 and unit 1
    ASSERT_EQ (rlv12Device.unit (0)->attach ("rl01_0.dsk",
            Bitmask(AttachFlags::NewFile | AttachFlags::Overwrite)), 
        StatusCode::OK);
    ASSERT_EQ (rlv12Device.unit (1)->attach ("rl01_1.dsk",
            Bitmask(AttachFlags::NewFile | AttachFlags::Overwrite)), 
        StatusCode::OK);

    // Clear errors and volume check condition for both units
    rlv12Device.writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);
    rlv12Device.writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);
    rlv12Device.writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive1);

    // Verify the controller is ready to perform an operation (the drive
    // does not have to be ready)
    u16 result;
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & CSR_ControllerReady, CSR_ControllerReady);

    // Load DAR with seek parameters
    rlv12Device.writeWord (RLDAR, DAR_Marker | DAR_Seek | DAR_DirectionOut |
        DAR_cylinderDifference (5));

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Seek Command (03)
    // in the function bits.
    rlv12Device.writeWord (RLCSR, CSR_SeekCommand | CSR_Drive0);

    // Verify the controller is ready to accept a command for another unit
    // while the drive is not ready
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady);

    // Start the same seek command for unit 1
    rlv12Device.writeWord (RLCSR, CSR_SeekCommand | CSR_Drive1);

    // Verify the controller is ready and no errors are indicated
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_CompositeError), 
        CSR_ControllerReady);

    // Wait for command completion
    std::this_thread::sleep_for (std::chrono::milliseconds (500));

    // Verify now both controller and drive 1 are ready
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady | CSR_Drive1), 
        CSR_ControllerReady | CSR_DriveReady | CSR_Drive1);

    // Verify the cylinder position
    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Read Header Command (04)
    // in the function bits.
    rlv12Device.writeWord (RLCSR, CSR_ReadHeaderCommand | CSR_Drive1);

    // Wait for command completion
    std::this_thread::sleep_for (std::chrono::milliseconds (500));

    // Verify the controller is ready
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result, CSR_ControllerReady | CSR_ReadHeaderCommand | 
        CSR_DriveReady | CSR_Drive1);

    // Expected result in the MPR register: Sector Address, Head Select and
    // Cylinder Address
    u16 mpr;
    rlv12Device.read (RLMPR, &mpr);
    ASSERT_EQ (MPR_cylinder (mpr), 5);
}

// Verify a seek command command can be started while a seek is in progress.
TEST_F (RLV12SeekTest, seekOnBusyDriveAccepted)
{
    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device.unit (0)->attach ("rl01.dsk",
            Bitmask(AttachFlags::NewFile | AttachFlags::Overwrite)), 
        StatusCode::OK);

    // Clear errors and volume check condition
    rlv12Device.writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);
    rlv12Device.writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

    // Verify controller and drive are ready 
    u16 result;
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & CSR_ControllerReady | CSR_DriveReady,
        CSR_ControllerReady | CSR_DriveReady);

    // Load DAR with seek parameters
    rlv12Device.writeWord (RLDAR, DAR_Marker | DAR_Seek | DAR_DirectionOut |
        DAR_cylinderDifference (5));

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Seek Command (03)
    // in the function bits.
    rlv12Device.writeWord (RLCSR, CSR_SeekCommand);

    // Verify the controller is ready to accept a command
    // while the drive is not ready
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady);

    // Issue another seek command to the same drive
    rlv12Device.writeWord (RLCSR, CSR_SeekCommand);

    // Wait till both seeks are completed
    std::this_thread::sleep_for (std::chrono::milliseconds (500));

    // Verify both controller and drive are ready
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // Get the current track, head and cylinder
    rlv12Device.writeWord (RLCSR, CSR_ReadHeaderCommand);

    // Wait for command completion
    std::this_thread::sleep_for (std::chrono::milliseconds (500));

    // Verify both seeks are executed
    u16 mpr;
    rlv12Device.read (RLMPR, &mpr);
    ASSERT_EQ (MPR_cylinder (mpr), 10);
}
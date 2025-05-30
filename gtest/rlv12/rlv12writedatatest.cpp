#include "pdp11/pdp11.h"
#include "msv11d/msv11d.h"
#include "rlv12/rlv12.h"
#include "configdata/rl/rlunitconfig/rlunitconfig.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <gtest/gtest.h>
#include <memory>

using std::make_shared;

using namespace std::chrono_literals;

// Definition of the test fixture
class RLV12WriteDataTest : public ::testing::Test
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
    static constexpr u16  CSR_DriveError          = (1 << 14);
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
    static constexpr u16 MPR_BrushHome          = (1 << 3);
    static constexpr u16 MPR_HeadsOut           = (1 << 4);
    static constexpr u16 MPR_DriveSelectError   = (1 << 8);
    static constexpr u16 MPR_WriteGateError     = (1 << 10);
    static constexpr u16 MPR_WriteLock          = (1 << 13);
    static const u16 MPR_LockOn                 = 5;

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
            result = rlv12Device->read (RLCSR);
        }
        while (!(result & CSR_ControllerReady));
    }

    void SetUp() override
    {
        // Create a minimal system, conisting of just the bus, memory
        // and the RLV12 device to be tested.
        bus.installModule (msv11);
        bus.installModule (rlv12Device);

        // Make sure the controller has started
        waitForControllerReady ();
    }
};

// Verify the correct execution of Write Data command
TEST_F (RLV12WriteDataTest, writeDataSucceeds)
{
    // This unit configuration uses the default spin_up_time of zero seconds
    // so the drive immediately locks on to cylinder 0.
    RLUnitConfig writeDataSucceedsConfig
    ({
        .fileName = "rl01.dsk",
        .newFile = true,
        .overwrite = true
    });

    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device->unit (0)->init (make_shared<RLUnitConfig> (writeDataSucceedsConfig)), 
        StatusCode::Success);

    // Clear errors and volume check condition
    rlv12Device->writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);
    rlv12Device->writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

    waitForControllerReady ();

    // Fill 512 bytes of memory with the values to be written and a marker
    u16 address;
    for (address = 0; address < 512; address += 2)
        bus.writeWord (address, 0177777);
    bus.writeWord (address, 1);
    
    // Verify controller and drive are ready
    u16 result = rlv12Device->read (RLCSR);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady | CSR_DriveReady);

    // Point at memory address 0
    rlv12Device->writeWord (RLBAR, 0);

    // Load DAR with disk address zero
    rlv12Device->writeWord (RLDAR, 0);

    // Load MPR with two's complement for 256 words
    rlv12Device->writeWord (RLMPR, 0xFF00);

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Read Data Command (06)
    // in the function bits.
    rlv12Device->writeWord (RLCSR, CSR_WriteDataCommand);

    waitForControllerReady ();

    // Verify now both controller and drive are ready and no error is
    // indicated
    result = rlv12Device->read (RLCSR);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // Clear the memory
    for (address = 0; address < 512; address += 2)
        bus.writeWord (address, 0);

    // Read back the block from disk
    rlv12Device->writeWord (RLDAR, 0);
    rlv12Device->writeWord (RLBAR, 0);
    rlv12Device->writeWord (RLMPR, 0xFF00);
    rlv12Device->writeWord (RLCSR, CSR_ReadDataCommand);

    waitForControllerReady ();

    // Verify Read Data is executed without errors
    result = rlv12Device->read (RLCSR);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // Verify contents of the first 512 bytes of memory. The word at address
    // 512 should not be overwritten.
    u16 contents;
    for (address = 0; address < 512; address += 2)
    {
        contents = bus.read (address);
        ASSERT_EQ (contents, 0177777);
    }

    contents = bus.read (address);
    ASSERT_EQ (contents, 01);
}


// Verify the correct execution of Write Data command of less than 256 bytes
TEST_F (RLV12WriteDataTest, partialWriteDataSucceeds)
{
    // This unit configuration uses the default spin_up_time of zero seconds
    // so the drive immediately locks on to cylinder 0.
    RLUnitConfig partialWriteDataSucceedsConfig
    ({
        .fileName = "rl01.dsk",
        .newFile = true, 
        .overwrite = true
    });

    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device->unit (0)->init (make_shared<RLUnitConfig> (partialWriteDataSucceedsConfig)), 
        StatusCode::Success);

    // Clear errors and volume check condition
    rlv12Device->writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);
    rlv12Device->writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

    waitForControllerReady ();

    // Fill 256 bytes of memory with the values to be written and a marker
    u16 address;
    for (address = 0; address < 256; address += 2)
        bus.writeWord (address, 0177777);
    
    // Verify controller and drive are ready
    u16 result = rlv12Device->read (RLCSR);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady | CSR_DriveReady);

    // Write 100 words
    rlv12Device->writeWord (RLBAR, 0);
    rlv12Device->writeWord (RLDAR, 0);
    rlv12Device->writeWord (RLMPR, 0xFF9C);
    rlv12Device->writeWord (RLCSR, CSR_WriteDataCommand);

    waitForControllerReady ();

    // Verify now both controller and drive are ready and no error is
    // indicated
    result = rlv12Device->read (RLCSR);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // Fill memory with a value other than the value to be returned by
    // the controller (i.e. 0177777 and 0)
    for (address = 0; address < 256; address += 2)
        bus.writeWord (address, 1);

    // Read the complete sector back from the disk
    rlv12Device->writeWord (RLDAR, 0);
    rlv12Device->writeWord (RLBAR, 0);
    rlv12Device->writeWord (RLMPR, 0xFF00);
    rlv12Device->writeWord (RLCSR, CSR_ReadDataCommand);

    waitForControllerReady ();

    // Verify Read Data is executed without errors
    result = rlv12Device->read (RLCSR);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // The first 200 bytes should contain the value 017777, the remaining
    // 56 bytes should have the value 0.
    u16 contents;
    for (address = 0; address < 200; address += 2)
    {
        contents = bus.read (address);
        ASSERT_EQ (contents, 0177777);
    }

    for (address = 200; address < 256; address += 2)
    {
        contents = bus.read (address);
        ASSERT_EQ (contents, 0);
    }
}

TEST_F (RLV12WriteDataTest, writeDataOnWriteProtectedDriveFails)
{
    // This unit configuration uses the default spin_up_time of zero seconds
    // so the drive immediately locks on to cylinder 0. 
    RLUnitConfig writeDataSucceedsConfig
    ({
        .fileName = "rl01.dsk",
        .newFile = true,
        .overwrite = true,
        .writeProtect = true
        });

    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device->unit (0)->init (make_shared<RLUnitConfig> (writeDataSucceedsConfig)),
        StatusCode::Success);

    // Clear errors and volume check condition
    rlv12Device->writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);
    rlv12Device->writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

    waitForControllerReady ();

    // Fill 512 bytes of memory with the values to be written and a marker
    u16 address;
    for (address = 0; address < 512; address += 2)
        bus.writeWord (address, 0177777);
    bus.writeWord (address, 1);

    // Verify controller and drive are ready
    u16 result = rlv12Device->read (RLCSR);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // Point at memory address 0
    rlv12Device->writeWord (RLBAR, 0);

    // Load DAR with disk address zero
    rlv12Device->writeWord (RLDAR, 0);

    // Load MPR with two's complement for 256 words
    rlv12Device->writeWord (RLMPR, 0xFF00);

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Write Data Command
    // in the function bits. This write should fail as the drive is
    // write-protected.
    rlv12Device->writeWord (RLCSR, CSR_WriteDataCommand);

    waitForControllerReady ();

    // Verify now both controller and drive are ready and the CSR indicates
    // a drive error. Drive Ready should be negated when a drive error occurs
    // (EK-RLV12-TD-001 par. 3.2.2.7).
    result = rlv12Device->read (RLCSR);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_DriveError | CSR_ControllerReady | CSR_DriveReady),
         CSR_CompositeError | CSR_DriveError | CSR_ControllerReady);

    // Execute a Get Status command to verify a Write Gate Error is reported.
    // Load DAR with ones in bits 01 and 00, reset bit cleared and
    // zeros in the other locations
    rlv12Device->writeWord (RLDAR, DAR_GetStatus | DAR_Marker);

    // Load the CSR with drive-select bits for unit 3, a negative GO bit
    // (i.e. bit 7 cleared), interrups disabled and a Get Status Command (02)
    // in the function bits.
    rlv12Device->writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

    waitForControllerReady ();

    // Expected result in the MPR register: heads locked on a cylinder,
    // Write Gate Error and Drive Type RL01
    u16 mpr = rlv12Device->read (RLMPR);
    ASSERT_EQ (mpr, MPR_WriteLock | MPR_WriteGateError | MPR_HeadsOut |
        MPR_BrushHome | MPR_LockOn);
}

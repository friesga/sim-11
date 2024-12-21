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
class RLV12ReadDataTest : public ::testing::Test
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
            StatusCode::OK);

        // Clear errors and volume check condition
        rlv12Device->writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);
        rlv12Device->writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

        waitForControllerReady ();
    }
};

// Verify the correct execution of Read Data command
TEST_F (RLV12ReadDataTest, readDataSucceeds)
{
    // Fill 512 bytes of memory with the value 0xff and write a mark at
    // address 512 to be able to verify the memory is overwritten by the
    // Read Data command
    u16 address;
    for (address = 0; address < 512; address += 2)
        bus.writeWord (address, 0177777);
    bus.writeWord (address, 01);

    // Verify the controller is ready to perform an operation (the drive
    // does not have to be ready)
    u16 result;
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result & CSR_ControllerReady, CSR_ControllerReady);

    // Point at memory address 0
    rlv12Device->writeWord (RLBAR, 0);

    // Load DAR with disk address zero
    rlv12Device->writeWord (RLDAR, 0);

    // Load MPR with two's complement for 256 words
    rlv12Device->writeWord (RLMPR, 0xFF00);

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Read Data Command (06)
    // in the function bits.
    rlv12Device->writeWord (RLCSR, CSR_ReadDataCommand);

    waitForControllerReady ();

    // Verify now both controller and drive are ready
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady | CSR_DriveReady);

    // Verify contents of the first 512 bytes of memory. The word at address
    // 512 should not be overwritten.
    u16 contents;
    for (address = 0; address < 512; address += 2)
    {
        contents = bus.read (address);
        ASSERT_EQ (contents, 0);
    }

    contents = bus.read (address);
    ASSERT_EQ (contents, 01);
}

// Verify a Header Not Found error is returned if the head is not
// at the correct cylinder
TEST_F (RLV12ReadDataTest, readDataFails)
{
    // Verify the controller and drive are ready
    u16 result;
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady | CSR_DriveReady);

    // Seek to cylinder 5
    rlv12Device->writeWord (RLDAR, DAR_Marker | DAR_Seek | DAR_DirectionOut |
        DAR_cylinderDifference (5));
    rlv12Device->writeWord (RLCSR, CSR_SeekCommand);

    waitForControllerReady ();

    // Point at memory address 0
    rlv12Device->writeWord (RLBAR, 0);

    // Load DAR with disk address zero
    rlv12Device->writeWord (RLDAR, 0);

    // Load MPR with two's complement for 256 words
    rlv12Device->writeWord (RLMPR, 0xFF00);

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Read Data Command (06)
    // in the function bits.
    rlv12Device->writeWord (RLCSR, CSR_ReadDataCommand);

    waitForControllerReady ();

    // Verify the CSR indicates an error
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_CompositeError | CSR_OperationIncomplete),
        CSR_CompositeError | CSR_OperationIncomplete);
}

// Verify spiral reads result in an error
TEST_F (RLV12ReadDataTest, spiralReadFails)
{
    // Verify controller and drive are ready
    u16 result;
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady | CSR_DriveReady);

    // Fill two sectors of memory to be able to verify exactly one
    // sector is read
    u16 address;
    for (address = 0; address < 512; address += 2)
        bus.writeWord (address, 0177777);

    // Execute a read of two sectors (i.e. 256 words), starting at the last
    // sector of the track (i.e. sector 39)
    rlv12Device->writeWord (RLBAR, 0);
    rlv12Device->writeWord (RLDAR, 39);
    rlv12Device->writeWord (RLMPR, 0xFE00);
    rlv12Device->writeWord (RLCSR, CSR_ReadDataCommand);

    waitForControllerReady ();

    // Verify an error is reported
    rlv12Device->read (RLCSR, &result);
    ASSERT_EQ (result & CSR_CompositeError, CSR_CompositeError);

    // The first 256 bytes should be read from disk
    u16 contents;
    for (address = 0; address < 256; address += 2)
    {
        contents = bus.read (address);
        ASSERT_EQ (contents, 0);
    }

    // The next 256 bytes should not be read
    for (address = 256; address < 512; address += 2)
    {
        contents = bus.read (address);
        ASSERT_EQ (contents, 0177777);
    }
}
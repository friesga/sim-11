#include "lsi11/lsi11.h"
#include "msv11d/msv11d.h"
#include "rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <gtest/gtest.h>

// Write to unit tests.

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

    void waitForControllerReady ()
    {
        u16 result;
        do
        {
            std::this_thread::yield ();
            rlv12Device.read (RLCSR, &result);
        }
        while (!(result & CSR_ControllerReady));
    }

    void SetUp() override
    {
        // Create a minimal system, conisting of just the bus, memory
        // and the RLV12 device to be tested.
        bus.installModule (1, &msv11);
        bus.installModule (2, &rlv12Device);

        // Make sure the controller has started
        waitForControllerReady ();
    }
};



// Verify the correct execution of Write Data command
TEST_F (RLV12WriteDataTest, writeDataSucceeds)
{
    RlUnitConfig writeDataSucceedsConfig
    {
        .fileName = "rl01.dsk",
        .newFile = true,
        .overwrite = true
    };

    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device.unit (0)->configure (writeDataSucceedsConfig), 
        StatusCode::OK);

    // Clear errors and volume check condition
    rlv12Device.writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);
    rlv12Device.writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

    waitForControllerReady ();

    // Fill 512 bytes of memory with the values to be written and a marker
    u16 address;
    for (address = 0; address < 512; address += 2)
        bus.writeWord (address, 0177777);
    bus.writeWord (address, 1);
    
    // Verify controller and drive are ready
    u16 result;
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady | CSR_DriveReady);

    // Point at memory address 0
    rlv12Device.writeWord (RLBAR, 0);

    // Load DAR with disk address zero
    rlv12Device.writeWord (RLDAR, 0);

    // Load MPR with two's complement for 256 words
    rlv12Device.writeWord (RLMPR, 0xFF00);

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrupts disabled and a Read Data Command (06)
    // in the function bits.
    rlv12Device.writeWord (RLCSR, CSR_WriteDataCommand);

    waitForControllerReady ();

    // Verify now both controller and drive are ready and no error is
    // indicated
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // Clear the memory
    for (address = 0; address < 512; address += 2)
        bus.writeWord (address, 0);

    // Read back the block from disk
    rlv12Device.writeWord (RLDAR, 0);
    rlv12Device.writeWord (RLBAR, 0);
    rlv12Device.writeWord (RLMPR, 0xFF00);
    rlv12Device.writeWord (RLCSR, CSR_ReadDataCommand);

    waitForControllerReady ();

    // Verify Read Data is executed without errors
    rlv12Device.read (RLCSR, &result);
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
    RlUnitConfig partialWriteDataSucceedsConfig
    {
        .fileName = "rl01.dsk",
        .newFile = true,
        .overwrite = true
    };

    // Attach a new disk to unit 0
    ASSERT_EQ (rlv12Device.unit (0)->configure (partialWriteDataSucceedsConfig), 
        StatusCode::OK);

    // Clear errors and volume check condition
    rlv12Device.writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);
    rlv12Device.writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

    waitForControllerReady ();

    // Fill 256 bytes of memory with the values to be written and a marker
    u16 address;
    for (address = 0; address < 256; address += 2)
        bus.writeWord (address, 0177777);
    
    // Verify controller and drive are ready
    u16 result;
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady | CSR_DriveReady);

    // Write 100 words
    rlv12Device.writeWord (RLBAR, 0);
    rlv12Device.writeWord (RLDAR, 0);
    rlv12Device.writeWord (RLMPR, 0xFF9C);
    rlv12Device.writeWord (RLCSR, CSR_WriteDataCommand);

    waitForControllerReady ();

    // Verify now both controller and drive are ready and no error is
    // indicated
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // Fill memory with a value other than the value to be returned by
    // the controller (i.e. 0177777 and 0)
    for (address = 0; address < 256; address += 2)
        bus.writeWord (address, 1);

    // Read the complete sector back from the disk
    rlv12Device.writeWord (RLDAR, 0);
    rlv12Device.writeWord (RLBAR, 0);
    rlv12Device.writeWord (RLMPR, 0xFF00);
    rlv12Device.writeWord (RLCSR, CSR_ReadDataCommand);

    waitForControllerReady ();

    // Verify Read Data is executed without errors
    rlv12Device.read (RLCSR, &result);
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

#include "lsi11/lsi11.h"
#include "msv11d/msv11d.h"
#include "rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <gtest/gtest.h>

// Write to unit tests.

// Definition of the test fixture
class RLV12WriteCheckTest : public ::testing::Test
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
    static constexpr u16  CSR_WriteCheckCommand   = (01 << 1);
    static constexpr u16  CSR_GetStatusCommand    = (02 << 1);
    static constexpr u16  CSR_SeekCommand         = (03 << 1);
    static constexpr u16  CSR_ReadHeaderCommand   = (04 << 1);
    static constexpr u16  CSR_WriteDataCommand    = (05 << 1);
    static constexpr u16  CSR_ReadDataCommand     = (06 << 1);
    static constexpr u16  CSR_ControllerReady     = (1 << 7);
    
    static constexpr u16  CSR_CompositeError      = (1 << 15); 
    static constexpr u16  CSR_Drive0              = (0 << 8);
    static constexpr u16  CSR_Drive1              = (1 << 8);
    static constexpr u16  CSR_Drive2              = (2 << 8);
    static constexpr u16  CSR_Drive3              = (3 << 8);

    // Define CSR Error codes
    inline u16 CSR_ErrorCode (u16 csr) {return (csr >> 10) & 017;}
    static constexpr u16  CSR_OperationIncomplete = 1;
    static constexpr u16  CSR_WriteCheckEror      = 2;

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

        RlUnitConfig rlUnitConfig
        {
            .fileName = "rl01.dsk",
            .newFile = true,
            .overwrite = true
        };

        // Attach a new disk to unit 0
        ASSERT_EQ (rlv12Device.unit (0)->configure (rlUnitConfig), 
            StatusCode::OK);

        // Clear errors and volume check condition
        rlv12Device.writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);
        rlv12Device.writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive0);

        // Wait till Read Header command is completed
        waitForControllerReady ();

        // Fill 512 bytes of memory with the values to be written and a marker
        u16 address;
        for (address = 0; address < 512; address += 2)
            bus.writeWord (address, 0177777);
    }
};

// Verify the correct execution of Write Check command
TEST_F (RLV12WriteCheckTest, writeCheckSucceeds)
{
    // Verify controller and drive are ready
    u16 result;
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady | CSR_DriveReady);

    // Write 256 words (i.e. 2 sectors)
    rlv12Device.writeWord (RLBAR, 0);
    rlv12Device.writeWord (RLDAR, 0);
    rlv12Device.writeWord (RLMPR, 0xFF00);
    rlv12Device.writeWord (RLCSR, CSR_WriteDataCommand);

    waitForControllerReady ();

    // Verify both controller and drive are ready and no error is
    // indicated
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // Issue the Write Check command
    rlv12Device.writeWord (RLBAR, 0);
    rlv12Device.writeWord (RLDAR, 0);
    rlv12Device.writeWord (RLMPR, 0xFF00);
    rlv12Device.writeWord (RLCSR, CSR_WriteCheckCommand);

    waitForControllerReady ();

    // Verify the Write Check indicates no error
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);
}


// Verify the write check detects and reports an error
TEST_F (RLV12WriteCheckTest, writeCheckFails)
{
    // Verify controller and drive are ready
    u16 result;
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & (CSR_ControllerReady | CSR_DriveReady), 
        CSR_ControllerReady | CSR_DriveReady);

    // Write 256 words (i.e. 2 sectors)
    rlv12Device.writeWord (RLBAR, 0);
    rlv12Device.writeWord (RLDAR, 0);
    rlv12Device.writeWord (RLMPR, 0xFF00);
    rlv12Device.writeWord (RLCSR, CSR_WriteDataCommand);

    waitForControllerReady ();

    // Verify both controller and drive are ready and no error is
    // indicated
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & 
        (CSR_CompositeError | CSR_ControllerReady | CSR_DriveReady),
        CSR_ControllerReady | CSR_DriveReady);

    // Poke a memory address to evoke a write check error
    bus.writeWord (2, 1);

    // Issue the Write Check command
    rlv12Device.writeWord (RLBAR, 0);
    rlv12Device.writeWord (RLDAR, 0);
    rlv12Device.writeWord (RLMPR, 0xFF00);
    rlv12Device.writeWord (RLCSR, CSR_WriteCheckCommand);

    waitForControllerReady ();

    // Verify the CSR indicates the appropriate errors
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & CSR_CompositeError, CSR_CompositeError);
    ASSERT_EQ (CSR_ErrorCode (result), CSR_WriteCheckEror);
}
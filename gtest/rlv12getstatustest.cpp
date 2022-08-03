#include "lsi11/lsi11.h"
#include "msv11d/msv11d.h"
#include "rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <gtest/gtest.h>

// Write to unit tests.

// Definition of the test fixture
class RLV12GetStatusTest : public ::testing::Test
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
    static constexpr u16  CSR_GetStatusCommand    = (02 << 1);
    static constexpr u16  CSR_ControllerReady     = (1 << 7);
    static constexpr u16  CSR_OperationIncomplete = (1 << 10); 
    static constexpr u16  CSR_CompositeError      = (1 << 15); 
    static constexpr u16  CSR_Drive0              = 0;
    static constexpr u16  CSR_Drive1              = (1 << 8);
    static constexpr u16  CSR_Drive2              = (2 << 8);
    static constexpr u16  CSR_Drive3              = (3 << 8);

    // DAR bit definitions
    static constexpr u16 DAR_Marker    = (1 << 00);
    static constexpr u16 DAR_GetStatus = (1 << 01);
    static constexpr u16 DAR_Reset     = (1 << 03);

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
            "-q",   // quiet
            "-n",   // createNewFile
            "-o"    // ovewrite
        };
        CmdLineOptions::processOptions (sizeof (argvSet0) /sizeof (argvSet0[0]),
            argvSet0);
    }
};

// Verify a Get Status Command without the Get Status bit set fails
TEST_F (RLV12GetStatusTest, getStatusFails)
{
    // Verify the controller is ready to perform an operation (the drive
    // does not have to be ready)
    u16 result;
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & CSR_ControllerReady, CSR_ControllerReady);

    // Load DAR without Reset and Get Status bits
    // zeros in the other locations
    rlv12Device.writeWord (RLDAR, 0);

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrups disabled and a Get Status Command (02)
    // in the function bits.
    rlv12Device.writeWord (RLCSR, CSR_GetStatusCommand);

    // Verify the controller reports an Operation Incomplete
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result, CSR_CompositeError |
        CSR_OperationIncomplete | CSR_ControllerReady | CSR_GetStatusCommand);
}

// Verify the controller can be reset by means of the Get Status Command
TEST_F (RLV12GetStatusTest, resetSucceeds)
{
    // Verify the controller is ready to perform an operation (the drive
    // does not have to be ready)
    u16 result;
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & CSR_ControllerReady, CSR_ControllerReady);

    // Load DAR with ones in bits 01 and 00, reset bit cleared and
    // zeros in the other locations
    rlv12Device.writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);

    // Load the CSR with drive-select bits for unit 0, a negative GO bit
    // (i.e. bit 7 cleared), interrups disabled and a Get Status Command (02)
    // in the function bits.
    rlv12Device.writeWord (RLCSR, CSR_GetStatusCommand);

    // Expected result in the MPR register: No errors, Drive type RL01
    u16 mpr;
    rlv12Device.read (RLMPR, &mpr);
    ASSERT_EQ (mpr, 00);

    // Verify the controller is ready without error indications
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result, CSR_ControllerReady | CSR_GetStatusCommand);
}


// Verify the controller can be reset by means of the Get Status Command
TEST_F (RLV12GetStatusTest, drive3CanBeSelected)
{
    // Verify the controller is ready to perform an operation (the drive
    // does not have to be ready)
    u16 result;
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result & CSR_ControllerReady, CSR_ControllerReady);

    // Load DAR with ones in bits 01 and 00, reset bit cleared and
    // zeros in the other locations
    rlv12Device.writeWord (RLDAR, DAR_Reset | DAR_GetStatus | DAR_Marker);

    // Load the CSR with drive-select bits for unit 1, a negative GO bit
    // (i.e. bit 7 cleared), interrups disabled and a Get Status Command (02)
    // in the function bits.
    rlv12Device.writeWord (RLCSR, CSR_GetStatusCommand | CSR_Drive3);

    // Expected result in the MPR register: No errors, Drive type RL01
    u16 mpr;
    rlv12Device.read (RLMPR, &mpr);
    ASSERT_EQ (mpr, 00);

    // Verify the controller is ready without error indications
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result, CSR_ControllerReady | CSR_GetStatusCommand | CSR_Drive3);
}
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
    u16 const RLBASE = 0174400;
    u16 const RLCSR = RLBASE;
    u16 const RLBAR = RLBASE + 02;
    u16 const RLDAR = RLBASE + 04;
    u16 const RLMPR = RLBASE + 06;
    // A 16 bit system has no BAE register

    // CSR bit definitions
    u16 const CSR_GetStatusCommand = (02 << 01);
    u16 const CSR_ControllerReady = (1 << 07);

    // DAR bit definitions
    u16 const DAR_Marker    = (1 << 00);
    u16 const DAR_GetStatus = (1 << 01);
    u16 const DAR_Reset     = (1 << 03);


    // Create bus structure, an RLV12 device and install the device
    LSI11 lsi;
    MSV11D msv11;
    RLV12 rlv12Device {};

    void SetUp() override
    {
        // Create a minimal system, conisting of just the bus, memory
        // and the RLV12 device to be tested.
        lsi.bus.installModule (1, &msv11);
        lsi.bus.installModule (2, &rlv12Device);

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
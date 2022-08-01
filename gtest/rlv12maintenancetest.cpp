#include "lsi11/lsi11.h"
#include "msv11d/msv11d.h"
#include "rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <gtest/gtest.h>

// Write to unit tests.

// Definition of the test fixture
class RLV12MaintenanceTest : public ::testing::Test
{
protected:
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


TEST_F (RLV12MaintenanceTest, maintenance)
{
    // Use our own set of definitions to avoid common cause issues
    u16 const RLBASE = 0174400;
    u16 const RLCSR = RLBASE;
    u16 const RLBAR = RLBASE + 02;
    u16 const RLMPR = RLBASE + 06;
    // A 16 bit system has no BAE register

    u16 const MaintenanceMode = 0;

    // Set memory address, word count (-511) and issue maintenance
    // command.
    // The command is executed with no units attached as attaching a
    // unit will cause a Volume Check condition which results in error
    // bits set.
    rlv12Device.writeWord (RLBAR, 0);
    rlv12Device.writeWord (RLMPR, 0177001);
    rlv12Device.writeWord (RLCSR, MaintenanceMode);

    // Expected result: CSR CRDY (bit 7) set, all error bits cleared.
    u16 result;
    rlv12Device.read (RLCSR, &result);
    ASSERT_EQ (result, 0200);
}
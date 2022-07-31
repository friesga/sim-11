#include "lsi11/lsi11.h"
#include "rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <gtest/gtest.h>

// Definition of the test fixture
class RLV12Test : public ::testing::Test
{
protected:
    // Create bus structure, an RLV12 device and install the device
    LSI11 lsi;
    RLV12 rlv12Device {};

    void SetUp() override
    {
        lsi.bus.installModule (1, &rlv12Device);
    }
};

// Verify attaching a non-existing file returns the appropriate error
TEST_F (RLV12Test, attachReturnsOpenError)
{
    // Create bus structure, an RLV12 device and install the device
    // LSI11 lsi;
    // RLV12 rlv12Device {};
    // lsi.bus.installModule (1, &rlv12Device);

    ASSERT_EQ (rlv12Device.unit (0)->attach ("non-existingfile"), 
        StatusCode::OpenError);
}

/*
TEST_F (RLV12Test, attachOpensReadOnly)
{
    // Set read-only option
    char const *argvSet0[] =
    {
        "sim-11",
        "-r",   // readOnly
    };
    CmdLineOptions::processOptions (sizeof (argvSet0) /sizeof (argvSet0[0]),
        argvSet0);

    ASSERT_EQ (rlv12Device.unit (0)->attach ("non-existingfile"), 
        StatusCode::OpenError);
}
*/

TEST_F (RLV12Test, attachCreatesNewFile)
{
    // Set read-only option
    char const *argvSet0[] =
    {
        "sim-11",
        "-q",   // quiet
        "-n",   // createNewFile
    };
    CmdLineOptions::processOptions (sizeof (argvSet0) /sizeof (argvSet0[0]),
        argvSet0);

    // Remove a possibly existing file
    char fileName[] = "sim-11-attach-test";
    remove (fileName);

    // This creates a file in the default directory (out\build\<config>)
    ASSERT_EQ (rlv12Device.unit (0)->attach (fileName), 
        StatusCode::OK);
}
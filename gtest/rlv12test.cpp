#include "lsi11/lsi11.h"
#include "rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <gtest/gtest.h>

// Attach file to unit tests.
// 
// The following situations are possible:
// - Attach a non-existing file
// - Attach a file read-only (-r)
// - Attach a file read/write (default)
// - Attach a newly created file read/write (-n)
// - Attach a newly created already existing file (-n -o)
// - Attach a newly created file read only (-n -r)
// 
// These tests require that the unit flag UNIT_ATTABLE is set.
// 
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
    char fileName[] = "rl02.dsk";
    remove (fileName);

    // This creates a file in the default directory (out\build\<config>)
    ASSERT_EQ (rlv12Device.unit (0)->attach (fileName), 
        StatusCode::OK);
}

// This test requires the UNIT_ROABLE flag to be set and depends on the
// file created in the previous test.
TEST_F (RLV12Test, attachOpensReadOnly)
{
    // Set read-only option
    char const *argvSet0[] =
    {
        "sim-11",
        "-q",   // quiet
        "-r",   // readOnly
    };
    CmdLineOptions::processOptions (sizeof (argvSet0) /sizeof (argvSet0[0]),
        argvSet0);

    ASSERT_EQ (rlv12Device.unit (0)->attach ("rl02.dsk"), 
        StatusCode::OK);
}

// Verify an existing file is not overwritten without specification of
// the override option
TEST_F (RLV12Test, existingFileIsNotOverwritten)
{
    char const *argvSet0[] =
    {
        "sim-11",
        "-q",   // quiet
        "-n",   // createNewFile
    };
    CmdLineOptions::processOptions (sizeof (argvSet0) /sizeof (argvSet0[0]),
        argvSet0);

    ASSERT_EQ (rlv12Device.unit (0)->attach ("rl02.dsk"), 
        StatusCode::ArgumentError);
}

// Verify an existing file is overwritten when the override option is sety
TEST_F (RLV12Test, existingFileIsOverwritten)
{
    char const *argvSet0[] =
    {
        "sim-11",
        "-q",   // quiet
        "-n",   // createNewFile
        "-o"    // ovewrite
    };
    CmdLineOptions::processOptions (sizeof (argvSet0) /sizeof (argvSet0[0]),
        argvSet0);

    ASSERT_EQ (rlv12Device.unit (0)->attach ("rl02.dsk"), 
        StatusCode::OK);
}

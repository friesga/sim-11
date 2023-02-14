#include "lsi11/lsi11.h"
#include "rlv12/rlv12.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "unit/attachflags.h"

#include <gtest/gtest.h>
#include <memory>

using std::make_shared;

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
class RLV12AttachTest : public ::testing::Test
{
protected:
    // Create bus structure, an RLV12 device and install the device
    Qbus bus;
    RLV12 *rlv12Device = new RLV12 (&bus);

    void SetUp() override
    {
        bus.installModule (1, rlv12Device);
    }
};


// Verify attaching a non-existing file returns the appropriate error
TEST_F (RLV12AttachTest, attachReturnsOpenError)
{
    RLUnitConfig rlUnitConfig
    ({
        .fileName = "non-existingfile",
        .newFile = false,
        .readOnly = false
    });
 
    ASSERT_EQ (rlv12Device->unit (0)->configure (make_shared<RLUnitConfig> (rlUnitConfig)), 
        StatusCode::OpenError);
}


TEST_F (RLV12AttachTest, attachCreatesNewFile)
{
    RLUnitConfig attachCreatesNewFileConfig
    ({
        .fileName = "rl02.dsk",
        .newFile = true
    });

    // Remove a possibly existing file
    char fileName[] = "rl02.dsk";
    remove (fileName);

    // This creates a file in the default directory (out\build\<config>)
    ASSERT_EQ (rlv12Device->unit (0)->configure (make_shared<RLUnitConfig> (attachCreatesNewFileConfig)),
        StatusCode::OK);
}



// This test requires the UNIT_ROABLE flag to be set and depends on the
// file created in the previous test.
TEST_F (RLV12AttachTest, attachOpensReadOnly)
{
    RLUnitConfig attachOpensReadOnlyConfig
    ({
        .fileName = "rl02.dsk",
        .readOnly = true
    });

    ASSERT_EQ (rlv12Device->unit (0)->configure (make_shared<RLUnitConfig> (attachOpensReadOnlyConfig)), 
        StatusCode::OK);
}


// Verify an existing file is not overwritten without specification of
// the override option
TEST_F (RLV12AttachTest, existingFileIsNotOverwritten)
{
    RLUnitConfig existingFileIsNotOverwrittenConfig
    ({
        .fileName = "rl02.dsk",
        .newFile = true
    });

    ASSERT_EQ (rlv12Device->unit (0)->configure (make_shared<RLUnitConfig> (existingFileIsNotOverwrittenConfig)), 
        StatusCode::ArgumentError);
}

// Verify an existing file is overwritten when the override option is sety
TEST_F (RLV12AttachTest, existingFileIsOverwritten)
{
    RLUnitConfig existingFileIsOverwrittenConfig
    ({
        .fileName = "rl02.dsk",
        .overwrite = true
    });

    ASSERT_EQ (rlv12Device->unit (0)->configure (make_shared<RLUnitConfig> (existingFileIsOverwrittenConfig)),
        StatusCode::OK);
}

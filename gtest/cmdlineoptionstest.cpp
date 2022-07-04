#include "cmdlineoptions/cmdlineoptions.h"

#include <gtest/gtest.h>

// Define command line strings
char *argv[] =
{
    "sim-11",
    "-h"
};


TEST (CmdLineOptionsTest, processOptions)
{
    EXPECT_FALSE (CmdLineOptions::get().halt);

    CmdLineOptions::processOptions (sizeof (argv) /sizeof (argv[0]), argv);

    EXPECT_TRUE (CmdLineOptions::get().halt);
}
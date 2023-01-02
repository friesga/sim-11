#include "cmdlineoptions/cmdlineoptions.h"

#include <gtest/gtest.h>

// Define command line strings
char const *argvSet0[] =
{
    "sim-11",
    "-h",   // halt
    "-b",   // bootstrap
    "-z",   // compress
    "-x",   // exit_on_halt
    "-l",   // load_file
    "loadfile",
    "-t",   // trace_file
    "tracefile"
};

TEST (CmdLineOptionsTest, optionsProcessed)
{
    CmdLineOptions cmdLineOptions (sizeof (argvSet0) /sizeof (argvSet0[0]), argvSet0);

    EXPECT_TRUE (cmdLineOptions.halt);
    EXPECT_TRUE (cmdLineOptions.bootstrap);
    EXPECT_TRUE (cmdLineOptions.compress);
    EXPECT_TRUE (cmdLineOptions.exit_on_halt);
    EXPECT_FALSE (strcmp(cmdLineOptions.load_file, "loadfile"));
    EXPECT_FALSE (strcmp(cmdLineOptions.trace_file, "tracefile"));
}

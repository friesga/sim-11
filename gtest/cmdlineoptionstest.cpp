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
    "-r",   // readOnly
    "-n",   // createNewFile
    "-q",   // quiet
    "-l",   // load_file
    "loadfile",
    "-f",   // floppy_filename
    "floppyfilename",
    "-t",   // trace_file
    "tracefile"
};

TEST (CmdLineOptionsTest, optionsProcessed)
{
    EXPECT_FALSE (CmdLineOptions::get().halt);
    EXPECT_EQ (CmdLineOptions::get().floppy_filename, nullptr);
    EXPECT_EQ (CmdLineOptions::get().load_file, nullptr);
    EXPECT_FALSE (CmdLineOptions::get().bootstrap);
    EXPECT_EQ (CmdLineOptions::get().trace_file, nullptr);
    EXPECT_FALSE (CmdLineOptions::get().compress);
    EXPECT_FALSE (CmdLineOptions::get().exit_on_halt);
    EXPECT_FALSE (CmdLineOptions::get().readOnly);
    EXPECT_FALSE (CmdLineOptions::get().quiet);
    EXPECT_FALSE (CmdLineOptions::get().createNewFile);

    CmdLineOptions::processOptions (sizeof (argvSet0) /sizeof (argvSet0[0]), argvSet0);

    EXPECT_TRUE (CmdLineOptions::get().halt);
    EXPECT_TRUE (CmdLineOptions::get().bootstrap);
    EXPECT_TRUE (CmdLineOptions::get().compress);
    EXPECT_TRUE (CmdLineOptions::get().exit_on_halt);
    EXPECT_TRUE (CmdLineOptions::get().readOnly);
    EXPECT_TRUE (CmdLineOptions::get().quiet);
    EXPECT_TRUE (CmdLineOptions::get().createNewFile);
    EXPECT_FALSE (strcmp(CmdLineOptions::get().load_file, "loadfile"));
    EXPECT_FALSE (strcmp(CmdLineOptions::get().floppy_filename, "floppyfilename"));
    EXPECT_FALSE (strcmp(CmdLineOptions::get().trace_file, "tracefile"));
}

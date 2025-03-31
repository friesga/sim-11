#include "../cmdlineparser/cmdlineparser.h"
#include "trace/trace.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

void traceOption (string value)
{
    trace.activate (value.c_str (),
        Trace::Category::RLV12 |
        Trace::Category::Debug);
}

vector<CmdLineParser::CmdLineOption> options =
{{
    {"trace", traceOption},
}};

// A custom main() function for GoogleTest in which we can perform custom
// actions, such as enabling the tracing facility.
//
int main (int argc, char** argv)
{
    CmdLineParser cmdLineParser {options};

    // The testing::InitGoogleTest() function parses the command line for
    // GoogleTest flags, and removes all recognized flags. This function
    // must be called before calling RUN_ALL_TESTS(), or the flags won’t
    // be properly initialized.
    testing::InitGoogleTest (&argc, argv);

    // Process the application-specific options
    cmdLineParser.processCmdLine (argc, argv);

    // The main function must return the value of RUN_ALL_TESTS ().
    return RUN_ALL_TESTS ();
}
#include "rk11functionrecord.h"

#include <array>

using std::array;

string TraceRecord<RK11FunctionRecord>::getFunctionName () const
{
    static array<string, 8> const functionName =
    {
        "Control Reset",
        "Write",
        "Read",
        "Write Check",
        "Seek",
        "Read Check",
        "Drive Reset",
        "Write Lock"
    };

    return functionName[static_cast<int> (function_.operation)];
}
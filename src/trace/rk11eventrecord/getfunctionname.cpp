#include "rk11eventrecord.h"

#include <array>

using std::array;

string TraceRecord<RK11EventRecord>::getFunctionName (RKTypes::Function function) const
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

    return functionName[static_cast<int> (function.rkcs.operation)];
}
#include "rk11names.h"

#include <array>

using std::array;

string RK11Names::getFunctionName (RKTypes::Function function)
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

string RK11Names::getStateName (RK11D::State state)
{
    static array<string, 8> const stateName =
    {
        "WaitingForFunction",
        "ProcessingFunction",
        "Polling",
        "monostate"
    };

    return stateName[state.index ()];
}
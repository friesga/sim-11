#include "rk11eventrecord.h"

#include <array>

using std::array;

string TraceRecord<RK11EventRecord>::getStateName (RK11D::State state) const
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
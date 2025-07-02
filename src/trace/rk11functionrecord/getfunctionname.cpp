#include "rk11functionrecord.h"

const char* TraceRecord<RK11FunctionRecord>::getFunctionName ()
{
    const char* functionName[] =
    {
        "Control Reset",
        "Write",
        "Read"
        "WriteCheck",
        "Seek",
        "Read Check",
        "Drive Reset",
        "Write Lock"
    };

    return functionName[function_.operation];
}
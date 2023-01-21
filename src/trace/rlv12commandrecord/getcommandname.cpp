#include "rlv12commandrecord.h"

const char* TraceRecord<RLV12CommandRecord>::getCommandName ()
{
    const char* commandName[] =
    {
        "No-Op/Maintenance",
        "Write Check",
        "Get Status",
        "Seek",
        "Read Header",
        "Write Data",
        "Read Data",
        "Read Data Without Header Check"
    };

    return commandName[command_];
}
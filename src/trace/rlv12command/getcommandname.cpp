#include "rlv12command.h"

const char* TraceRecord<TraceRLV12Command>::getCommandName ()
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
#include "cmdprocessor.h"

//
// This function is a dummy as Get Status commands are not executed by the
// command processor and are executed in the RLV12 context. This function
// should therefore not be called.
//
u16 CmdProcessor::getStatusCmd (RL01_02 *unit, RLV12Command &rlv12Command)
{
    throw ("getStatusCmd call should not happen");
}
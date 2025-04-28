#include "rk05.h"

void RK05::processCommand (RKDefinitions::RKCommand rkCommand)
{
    sendTrigger (rkCommand);
}
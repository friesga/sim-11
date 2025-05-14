#include "rk05.h"

void RK05::processCommand (RKDefinitions::Function action)
{
    sendTrigger (action);
}
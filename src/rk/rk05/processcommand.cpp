#include "rk05.h"

void RK05::processCommand (RKTypes::Function action)
{
    sendTrigger (action);
}
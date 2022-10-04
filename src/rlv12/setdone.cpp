#include "rlv12.h"

// Set done and possibly errors
void RLV12::setDone (u16 status)
{
    rlcs |= status | CSR_DONE;
    if (rlcs & CSR_IE)
        bus->setInterrupt (TrapPriority::BR4, 4, vector_);
    else
        bus->clearInterrupt (TrapPriority::BR4, 4);
}
#include "rlv12.h"

// Set done and possibly errors
void RLV12::setDone (int32_t status)
{
    rlcs |= status | CSR_DONE;
    if (rlcs & CSR_IE)
        bus->setInterrupt (TrapPriority::BR4, 4, vector_);
    else
        bus->clearInterrupt (TrapPriority::BR4, 4);
}
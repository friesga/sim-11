#include "rlv12.h"

// Set done and possibly errors
void RLV12::setDone (RL01_2 &unit, u16 status)
{
    // Set Controller Ready and generate an interrupt if these are enabled
    // ToDo: clearInterrupt should not be necessary at this point?
    csr_ |= status | CSR_ControllerReady;
    if (csr_ & CSR_InterruptEnable)
        bus_->setInterrupt (TrapPriority::BR4, 4, vector_);
    else
        bus_->clearInterrupt (TrapPriority::BR4, 4);
}
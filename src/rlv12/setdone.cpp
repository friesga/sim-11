#include "rlv12.h"

// Set done and possibly errors
void RLV12::setDone (RL01_02 &unit, u16 status)
{
    // Set Controller Ready and generate an interrupt if these are enabled
    // ToDo: clearInterrupt should not be necessary at this point?
    csr_ |= status | RLV12const::CSR_ControllerReady;
    if (csr_ & RLV12const::CSR_InterruptEnable)
        bus_->requestInterrupt (InterruptPriority::BR4, 4, 0, vector_);
    else
        bus_->clearInterrupt (InterruptPriority::BR4, 0, 4);
}
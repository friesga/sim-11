#include "rlv12.h"

// Set done and possibly errors
void RLV12::setDone (RL01_2 &unit, u16 status)
{
    // Set/clear the Drive Ready and Drive Error bits from the currently
    // selected unit and set/clear the Composite Error bit.
    adjustDriveStatus (unit);
    adjustCompositeErrorBit ();

    // Set Controller Ready and generate an interrupt if these are enabled
    // ToDo: clearInterrupt should not be necessary at this point?
    rlcs |= status | CSR_DONE;
    if (rlcs & CSR_IE)
        bus->setInterrupt (TrapPriority::BR4, 4, vector_);
    else
        bus->clearInterrupt (TrapPriority::BR4, 4);
}
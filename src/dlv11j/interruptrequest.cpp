#include "dlv11j.h"

// Generate an DLV11J interrupt request
InterruptRequest DLV11J::interruptRequest(unsigned char vector)
{
    return InterruptRequest{RequestType::IntrptReq, TrapPriority::BR4, 0, vector};
}

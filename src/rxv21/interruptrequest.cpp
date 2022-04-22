#include "rxv21.h"

// Generate an RXV21 interrupt request
InterruptRequest RXV21::interruptRequest(unsigned char vector)
{
    return InterruptRequest{RequestType::IntrptReq, TrapPriority::BR4, 0, vector};
}

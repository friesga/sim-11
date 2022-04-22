#include "interruptrequest.h"

// Constructors
InterruptRequest::InterruptRequest()
    :
    priority_{TrapPriority::None},
    busOrder_{0},
    vector_{0}
{}

InterruptRequest::InterruptRequest(RequestType requestType, 
    TrapPriority priority, unsigned char busOrder, unsigned char vector)
    :
    requestType_ {requestType},
    priority_ {priority},
    busOrder_ {busOrder},
    vector_ {vector}
{}


// Operator less than, used to determine the priority of the interrupt request.
// The priority is based on first the trap priority and if these are equal by
// the order on the bus. The higher the number, the greater the priority.
bool InterruptRequest::operator< (InterruptRequest const &ir) const
{
    return (static_cast<long>(priority_) * 256) + busOrder_ <
        (static_cast<long>(ir.priority_) * 256) + ir.busOrder_;
}

// Accessors
RequestType InterruptRequest::requestType() const
{
    return requestType_;
}

unsigned char InterruptRequest::busOrder() const
{
    return busOrder_;
}

unsigned char InterruptRequest::vector() const
{
    return vector_;
}

TrapPriority InterruptRequest::priority() const
{
    return priority_;
}
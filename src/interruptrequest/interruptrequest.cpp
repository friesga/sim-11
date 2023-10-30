#include "interruptrequest.h"

// Constructors
InterruptRequest::InterruptRequest()
    :
    priority_{TrapPriority::None},
    busOrder_{0},
    vector_{0}
{}

InterruptRequest::InterruptRequest(TrapPriority priority,
        unsigned char busOrder, unsigned char vector)
    :
    priority_ {priority},
    busOrder_ {busOrder},
    vector_ {vector}
{}


// Operator less than, used to determine the priority of the interrupt request.
// The priority is based on first the trap priority and if these are equal by
// the order on the bus. The higher the number, the greater the priority.
bool InterruptRequest::operator< (InterruptRequest const &ir) const
{
    return intrptPriority (priority_, busOrder_) <
        intrptPriority (ir.priority_, ir.busOrder_);
}

bool InterruptRequest::operator== (InterruptRequest const &ir) const
{
    return intrptPriority (priority_, busOrder_) ==
        intrptPriority (ir.priority_, ir.busOrder_);
}

// Calculate the interrupt priority, based on first the trap priority and if
// these are equal by the order on the bus. The higher the number, the greater
// the priority.
long InterruptRequest::intrptPriority (TrapPriority trapPriority, 
    unsigned char busOrder) const
{
    return (static_cast<long>(trapPriority) * 256) + busOrder;
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
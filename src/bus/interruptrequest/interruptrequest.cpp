#include "interruptrequest.h"

// Constructors
InterruptRequest::InterruptRequest()
    :
    priority_ {TrapPriority::None},
    busOrder_ {0},
    functionOrder_ {0},
    vector_{0},
    ack_ {nullptr}
{}

InterruptRequest::InterruptRequest(TrapPriority priority,
        unsigned char busOrder, u8 functionOrder, unsigned char vector,
        function<void ()> ack)
    :
    priority_ {priority},
    busOrder_ {busOrder},
    functionOrder_ {functionOrder},
    vector_ {vector},
    ack_ {ack}
{}


// Operator less than, used to determine the priority of the interrupt request.
// The priority is based on first the trap priority and if these are equal by
// the order on the bus. The higher the number, the greater the priority.
bool InterruptRequest::operator< (InterruptRequest const &ir) const
{
    return intrptPriority (priority_, busOrder_, functionOrder_) <
        intrptPriority (ir.priority_, ir.busOrder_, ir.functionOrder_);
}

bool InterruptRequest::operator== (InterruptRequest const &ir) const
{
    return intrptPriority (priority_, busOrder_, functionOrder_) ==
        intrptPriority (ir.priority_, ir.busOrder_, functionOrder_);
}

// Calculate the interrupt priority, based on first the trap priority and if
// these are equal by the order on the bus. The higher the number, the greater
// the priority.
long InterruptRequest::intrptPriority (TrapPriority trapPriority, 
    unsigned char busOrder, u8 functionOrder) const
{
    return static_cast<long> (trapPriority) * 256 + 
        busOrder * 256 + functionOrder;
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

void InterruptRequest::acknowledge ()
{
    if (ack_)
        ack_();
}
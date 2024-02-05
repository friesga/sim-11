#ifndef _INTERRUPTREQUEST_H_
#define _INTERRUPTREQUEST_H_

#include "types.h"

// Definition of trap priorities. The BR4-BR7 priorities concur with
// the CPU priority as indicated in the PSW (bits 5-7).
enum class TrapPriority
{
    BusError = 12,
    InstructionTrap = 11,
    TraceTrap = 10,
    PowerFail = 9,
    Event = 8,
    BR7 = 7,
    BR6 = 6,
    BR5 = 5,
    BR4 = 4,
    None = 0
};

// This class comprises all information for an interrupt request:
// - Whether the request is a trap or an interrupt,
// - The trap priority,
// - The bus order. With equal bus request levels, devices on the bus
//   closer to the processor have higher priority,
// - The trap/interrupt vector.
//
// The vector range is 0 to 0376 and the maximum number of vectors therefore
// is 256. This number is therefore also the maximum number of devices in a
// PDP-11.
class InterruptRequest
{
public:
    InterruptRequest();
    InterruptRequest(TrapPriority priority, unsigned char busOrder,
        u8 functionOrder, unsigned char vector);
    bool operator< (InterruptRequest const &ir) const;
    bool operator== (InterruptRequest const &ir) const;

    // Accessors
    TrapPriority priority() const;
    unsigned char busOrder() const;
    unsigned char vector() const;

private:
    TrapPriority priority_;
    unsigned char busOrder_;
    u8 functionOrder_;
    unsigned char vector_;

    long intrptPriority (TrapPriority trapPriority, unsigned char busOrder,
        u8 functionOrder) const;
};

#endif // !_INTERRUPTREQUEST_H_

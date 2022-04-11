#ifndef _INTERRUPTREQUEST_H_
#define _INTERRUPTREQUEST_H_

// Definition of trap priorities. The actual numbers are of no importance,
// they just define the order of the priorities, leaving room for
// extension.
enum class TrapPriority
{
    BusError = 9,
    TraceTrap = 8,
    PowerFail = 7,
    Event = 6,
    BR7 = 5,
    BR6 = 4,
    BR5 = 3,
    BR4 = 2,
    InstructionTrap = 1,
    None = 0
};

// This class comprises all information for an interrupt request:
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
    TrapPriority trapPriority_;
    unsigned char busOrder_;
    unsigned char vector_;

public:
    InterruptRequest();
    InterruptRequest(TrapPriority trapPriority,
        unsigned char prioLevel, unsigned char vector);
    bool operator< (InterruptRequest const &ir) const;

    // Accessors
    TrapPriority trapPriority() const;
    unsigned char busOrder() const;
    unsigned char vector() const;

    /*
    // Definitions of the possible interrupt requests
    static InterruptRequest const busError;
    static InterruptRequest const traceTrap;
    static InterruptRequest const event;
    static InterruptRequest const BPT;
    static InterruptRequest const IOT;
    static InterruptRequest const EMT;
    static InterruptRequest const TRP;
    static InterruptRequest const none;
    */
};



#endif // !_INTERRUPTREQUEST_H_

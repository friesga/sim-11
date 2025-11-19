#ifndef _INTERRUPTREQUEST_H_
#define _INTERRUPTREQUEST_H_

#include "types.h"

#include <functional>
#include <atomic>

using std::function;
using std::atomic_uint;

// Definition of interrupt priorities. The BR4-BR7 priorities concur with
// the CPU priority as indicated in the PSW (bits 5-7).
enum class InterruptPriority
{
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
// - The trap/interrupt vector,
// - An optional callback function, acknowledging the processing of the
//   interrupt.
//
// The vector range is 0 to 0376 and the maximum number of vectors therefore
// is 128. This number is therefore also the maximum number of devices in a
// PDP-11.
class InterruptRequest
{
public:
    // Constructors
    InterruptRequest ();
    InterruptRequest (InterruptPriority priority, unsigned char busOrder,
        u8 functionOrder, u16 vector, function<void ()> requestGrant = 0);

    bool operator< (InterruptRequest const &ir) const;
    bool operator== (InterruptRequest const &ir) const;

    // Accessors
    InterruptPriority priority() const;
    unsigned char busOrder() const;
    unsigned char vector() const;
    void requestGrant ();
    unsigned int id () const;

private:
    InterruptPriority priority_;
    unsigned char busOrder_;
    u8 functionOrder_;
    u16 vector_;
    function<void ()> requestGrant_;
    unsigned int intrptReqId_;

    // The intrptSeqNr_ is used to uniquely identify interrupt requests in
    // the trace log. It is incremented atomically for each created interrupt
    // request. As an unsigned int, the id will overflow to zero when it
    // reaches its maximum value. An unsigned int is used to avoid unreadable
    // long id's.
    static atomic_uint intrptSeqNr_;

    long intrptPriority (InterruptPriority trapPriority, unsigned char busOrder,
        u8 functionOrder) const;
};

#endif // !_INTERRUPTREQUEST_H_

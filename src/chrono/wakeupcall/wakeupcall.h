#ifndef _WAKEUPCALL_H_
#define _WAKEUPCALL_H_

#include <cstdint>

// The currentTime is the time elapsed in micro seconds from the start
/// of the simulator. This durartion is given as an integer to prevent
// circular dependencies. Its only purpose is to discriminate real from
// fake wakeups.
class WakeUpCall
{
public:
    virtual void ring (uint64_t currentTime) = 0;
    virtual void waitFor () = 0;
};

#endif // _WAKEUPCALL
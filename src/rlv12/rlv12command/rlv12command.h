#ifndef _RLV12COMMAND_H_
#define _RLV12COMMAND_H_

#include "types.h"
#include "qbus/qbus.h"

#include <cstdint>      // For int32_t

class RLV12;
class RL01_2;

class RLV12Command
{
protected:
    int32_t diskAddress_;
    int32_t memoryAddress_;
    size_t wordCount_;

public:
    RLV12Command (int32_t diskAddress, int32_t memoryAddress, 
        size_t wordCount);

    // Accessors
    int32_t diskAddress() const;
    int32_t memoryAddress() const;
    size_t wordCount () const;

    // Return the position of the in this command specified track
    // as an offset in the file
    int32_t filePosition () const;

    // RLV12Command's are executed in the context of the controller and a unit
    virtual void execute (RLV12 *controller, RL01_2 *unit) = 0;
    virtual void finish (RLV12 *controller, RL01_2 *unit);
};

#endif // !_RLV12COMMAND_H_

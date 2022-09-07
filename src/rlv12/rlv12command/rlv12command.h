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
    int32_t trackNumber_;
    int32_t sectorNumber_;
    int32_t memoryAddress_;
    int32_t wordCount_;

public:
    RLV12Command (int32_t trackNr, int32_t sectorNr, 
        int32_t memoryAddress, int32_t wordCount);

    // Accessors
    int32_t trackNumber() const;
    int32_t sectorNumber() const;
    int32_t memoryAddress() const;
    int32_t wordCount () const;

    // Return the position of the in this command specified track
    // as an offset in the file
    int32_t filePosition () const;

    // RLV12Command's are executed in the context of the controller and a unit
    virtual void execute (RLV12 *controller, RL01_2 *unit) = 0;
    virtual void finish (RLV12 *controller, RL01_2 *unit);
};

#endif // !_RLV12COMMAND_H_

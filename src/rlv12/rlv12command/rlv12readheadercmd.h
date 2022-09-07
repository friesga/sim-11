#ifndef _RLV12READHEADERCMD_H_
#define _RLV12READHEADERCMD_H_

#include "rlv12command.h"

class RLV12ReadHeaderCmd : public RLV12Command
{
public:
    // Use base class constructor
    using RLV12Command::RLV12Command;

    void execute (RLV12 *controller, RL01_2 *unit) override;
    void finish (RLV12 *controller, RL01_2 *unit) override;
};

#endif // !_RLV12READHEADERCMD_H_

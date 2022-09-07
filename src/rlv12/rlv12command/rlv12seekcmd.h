#ifndef _RLV12SEEKCMD_H_
#define _RLV12SEEKCMD_H_

#include "rlv12command.h"

class RLV12SeekCmd : public RLV12Command
{
public:
    // Use base class constructor
    using RLV12Command::RLV12Command;

    void execute (RLV12 *controller, RL01_2 *unit) override;
};

#endif // !_RLV12READCMD_H_

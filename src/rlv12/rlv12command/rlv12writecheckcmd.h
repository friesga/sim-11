#ifndef _RLV12WRITECHECKCMD_H_
#define _RLV12WRITECHECKCMD_H_

#include "rlv12command.h"

#include <iostream>

class RLV12WriteCheckCmd : public RLV12Command
{
public:
    // Use base class constructor
    using RLV12Command::RLV12Command;

    void execute (RLV12 *controller, RL01_2 *unit) override;
};

#endif // !_RLV12WRITECHECKCMD_H_

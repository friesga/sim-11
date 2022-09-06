#ifndef _RLV12READNOHEADERCMD_H_
#define _RLV12READNOHEADERCMD_H_

#include "rlv12command.h"

#include <iostream>

class RLV12ReadNoHeaderCmd : public RLV12Command
{
public:
    // Use base class constructor
    using RLV12Command::RLV12Command;

    void execute () override
    {
        std::cout << "Execute RLV12ReadNoHeaderCmd\n";
    }
};

#endif // !_RLV12READNOHEADERCMD_H_

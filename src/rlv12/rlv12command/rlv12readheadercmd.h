#ifndef _RLV12READHEADERCMD_H_
#define _RLV12READHEADERCMD_H_

#include "rlv12command.h"

#include <iostream>

class RLV12ReadHeaderCmd : public RLV12Command
{
public:
    // Use base class constructor
    using RLV12Command::RLV12Command;

    void execute () override
    {
        std::cout << "Execute RLV12ReadHeaderCmd\n";
    }
};

#endif // !_RLV12READHEADERCMD_H_

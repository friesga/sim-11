#ifndef _RLV12WRITECMD_H_
#define _RLV12WRITECMD_H_

#include "rlv12command.h"

#include <iostream>

class RLV12WriteCmd : public RLV12Command
{
public:
    // Use base class constructor
    using RLV12Command::RLV12Command;

    void execute () override
    {
        std::cout << "Execute RLV12WriteCmd\n";
    }
};

#endif // !_RLV12WRITECMD_H_

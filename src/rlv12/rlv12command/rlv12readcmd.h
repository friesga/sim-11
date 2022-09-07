#ifndef _RLV12READCMD_H_
#define _RLV12READCMD_H_

#include "rlv12command.h"

#include <iostream>

class RLV12ReadCmd : public RLV12Command
{
public:
    // Use base class constructor
    using RLV12Command::RLV12Command;

    void execute (u16 *buffer, FILE *filePtr, u16 *rlcs, QBUS *bus) override;
};

#endif // !_RLV12READCMD_H_

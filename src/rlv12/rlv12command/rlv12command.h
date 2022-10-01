#ifndef _RLV12COMMAND_H_
#define _RLV12COMMAND_H_

#include "types.h"

#include <cstdint>

struct RLV12Command
{
    u8 function_;
    int32_t diskAddress_;
    int32_t memoryAddress_;
    size_t wordCount_;

    // Constructor
    RLV12Command (u8 function_, int32_t diskAddress, 
        int32_t memoryAddress,  size_t wordCount);
};

#endif // _RLV12COMMAND_H_
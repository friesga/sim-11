#ifndef _RLV12COMMAND_H_
#define _RLV12COMMAND_H_

#include "types.h"
#include "busaddress/busaddress.h"

#include <cstdint>
#include <cstddef>

struct RLV12Command
{
    u8 function_;
    int32_t diskAddress_;
    BusAddress<> memoryAddress_;
    u32 wordCount_;

    // Constructor
    RLV12Command (u8 function_, int32_t diskAddress, 
        BusAddress<> memoryAddress,  u32 wordCount);
};

#endif // _RLV12COMMAND_H_
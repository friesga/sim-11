#ifndef _RLCONFIG_H_
#define _RLCONFIG_H_

#include "types.h"

#include <cstddef>

// Definition of the configuration of the RL controller.
// 
// This struct decouples the RLProcesser class from the RLV12 class.

enum class RLType
{
    RL11,
    RLV11,
    RLV12
};

struct RLConfig
{
    u32 baseAddress_;
    u32 vector_;
    RLType rlType_; 
    bool _22bit_;
    size_t numUnits_;
    
    RLConfig (u32 baseAddress_, u32 vector, RLType rlType, 
        bool _22bit, size_t numUnits);
};
#endif // !_RLCONFIG_H_

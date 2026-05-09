#ifndef _MM11ECONFIG_H_
#define _MM11ECONFIG_H_

#include "devicetype.h"
#include "types.h"

struct MM11EConfig : public DeviceType<BusType::Unibus>
{
    // The MM11-E address can start at any 4K bank boundary. The
    // address configured is the starting address for the contiguous portion
    // of memory (4K, 8K, 16K, or 32K) contained on the module.
    u32 startingAddress {0};
};

#endif // _MM11ECONFIG_H_
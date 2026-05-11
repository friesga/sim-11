#ifndef _MM11ECONFIG_H_
#define _MM11ECONFIG_H_

#include "devicetype.h"
#include "types.h"

// This structure defines the configuration for an MM11-E(X) memory module. The MM11-E
// contains 4KW of memory, while the MM11-EX contains 8KW. The starting address for the
// module can be configured to be any 4K bank boundary.
//
struct MM11EConfig : public DeviceType<BusType::Unibus>
{
    // The MM11-E(X) address can start at any 4K bank boundary. The
    // address configured is the starting address for the contiguous portion
    // of memory (4KW or 8KW) contained on the module.
    u32 startingAddress {0};
    u16 memorySizeInBytes {8192}; 
};

#endif // _MM11ECONFIG_H_
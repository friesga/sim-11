#ifndef _MSV11CONFIG_H_
#define _MSV11CONFIG_H_

#include "types.h"

struct MSV11Config
{
    // Definition of the power source:
    // - System
    //   The MSV11 is operated with normal system power. This implies that
    //   when the power is switched off the memory contents are lost.
    // - BatteryBackup
    //   The system is equipped with a battery packup power source. When power
    //   is lost the memory contents are retained. This mode is necessary
    //   for the proper operation of power fail functionality.
    //
    enum class PowerSource
    {
        System,
        BatteryBackup
    };

    // MSV11 modules are factory-configured with power jumpers installed for
    // normal system power, only. (EK-MSV1D-OP-001)
    PowerSource powerSource {PowerSource::System};

    // The MSV11-D or MSV11-E address can start at any 4K bank boundary. The
    // address configured is the starting address for the contiguous portion
    // of memory (4K, 8K, 16K, or 32K) contained on the module. [...]
    // Note that the module is designated to accommodate a 128K system
    // addressing capability. (EK-MSV1D-OP-001)
    u32 startingAddress {0};

    // Factory-configured modules will not respond to Bank 7 addresses. In
    // special applications that permit the use of the lower 2K portion of
    // Bank 7 for system memory, the lower 2K portion of Bank 7 can be enabled
    // by setting the bank7_lower2K_enabled option to true.
    bool bank7Lower2kWEnabled {false};
};

#endif // _MSV11CONFIG_H_
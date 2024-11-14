#ifndef _MS11PCONFIG_H_
#define _MS11PCONFIG_H_

#include "types.h"

// Via the Unibus, 131,072 (128K) words or 262,144 (256K) bytes (18 bits) can
// be addressed. Via the Extended Unibus 2,097,152 (2048K) words or
// 4,194,304 (4096K) bytes (22 bits) can be addressed.
//
// The MS11-PB (M8743-A) is MOS RAM, which provides 512KW of data storage.
//
struct MS11PConfig
{
    // MOS storage devices are volatile (data is not retained when power is
    // lost), so DIGITAL provides an optial battery backup unit to support
    // the MOS power supply regulator(s) (MS11-P MOS Memory Technical
    // Manual, EK-MS11P-TM-001).
    // 
    // Definition of the power source:
    // - System
    //   The MS11P is operated with normal system power. This implies that
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

    PowerSource powerSource {PowerSource::System};

    // The memory starting address (MSA) is the lowest bus address to which
    // the MS11-P responds. You must assign the starting address to an 8K
    // [word] boundary within the 2048K [word] extended Unibus address space.
    // (EK-MS11P-TM-001)
    u32 startingAddress {0};

    // Address decoding logic in the MS11-P specifies the control and status
    // register (CSR) [is] in the 17772100 - 17772136 range. However, it helps
    // to assign CSR addresses to modules in the same order as the memory
    // starting addresses. Each memory module's CSR is assigned a specific
    // address (EK-MS11P-TM-001).
    // This implies the default CSR address is 017772100.
    u32 csrAddress {017772100};
};

#endif // _MS11PCONFIG_H_
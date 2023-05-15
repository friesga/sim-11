#ifndef _MSV11CONFIG_H_
#define _MSV11CONFIG_H_

#include "../deviceconfig/deviceconfig.h"

struct MSV11Config : public DeviceConfig
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
    MSV11Config ();
};

#endif // _MSV11CONFIG_H_
#ifndef _KDF11_UCONFIG_H_
#define _KDF11_UCONFIG_H_

#include "devicetype.h"
#include "configdata/kd11config/kd11config.h"
#include "configdata/kdf11/slu/sluconfig/sluconfig.h"

#include <memory>

using std::shared_ptr;

struct KDF11_UConfig : public DeviceType<BusType::Unibus>
{
    // Jumper W2 controls the powerfail power-up action:
    // 
    // Boot   - Boot on powerup from powerfail,
    // 
    // Vector - Power up via location 024 provided the optional battery backup
    //          unit is installed and functioning
    //
    enum class PowerUpMode
    {
        Boot,
        Vector
    };

    // Jumper W3 controls kernel halt enable:
    // 
    // AllowHalt - In kernel mode allows HALT instruction to be executed. In
    //             all other modes, a HALT instruction traps to location 010.
    // 
    // IllegalInstructionTrap - In all modes, a HALT instruction traps to
    //                          location 010 (except during powerfail).
    //
    enum class KernelHaltMode
    {
        AllowHalt,
        IllegalInstructionTrap
    };

    PowerUpMode powerUpMode {PowerUpMode::Boot};
    KernelHaltMode kernelHaltMode {KernelHaltMode::AllowHalt};

    // Jumper W14 selects boot address when boot on powerup is enabled,
    // either 0165000 or 0173000.
    u16 bootAddress {0165000};
    shared_ptr<SLUConfig> sluConfig;

    KDF11_UConfig ();
};

#endif // _KDF11_UCONFIG_H_
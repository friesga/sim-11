#ifndef _BDV11CONFIG_H_
#define _BDV11CONFIG_H_

#include "../deviceconfig/deviceconfig.h"

struct BDV11Config : public DeviceConfig
{
    enum class BootROM
    {
        BDV11,      // Standard BDV11 Boot ROMs 23-045E2/046E2
        KDF11_BA    // KDF11-BA (PDP-11/23+) Boot ROMs 23-339E2/23-340E2
    };

    enum class BootDevice
    {
        RK05,
        RL01,
        TU58,
        RX01,
        RX02,
        BDV11ROM
    };

    bool cpuTests {true};
    bool memoryTests {true};
    bool decnetBoot {false};
    bool consoleDialog {true};
    BootROM bootROM {BootROM::BDV11};
    BootDevice bootDevice {BootDevice::RX02};

    BDV11Config ();
};

#endif // _BDV11CONFIG_H_
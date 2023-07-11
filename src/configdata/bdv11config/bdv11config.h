#ifndef _BDV11CONFIG_H_
#define _BDV11CONFIG_H_

#include "../deviceconfig/deviceconfig.h"

struct BDV11Config : public DeviceConfig
{
    enum class BootDevice
    {
        RK05,
        RL01,
        RX01,
        RX02,
        BDV11ROM
    };

    bool cpuTests {true};
    bool memoryTests {true};
    bool decnetBoot {false};
    bool consoleDialog {true};
    BootDevice bootDevice {BootDevice::RX02};

    BDV11Config ();
};

#endif // _BDV11CONFIG_H_
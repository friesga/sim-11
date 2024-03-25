#ifndef _KDF11_ACONFIG_H_
#define _KDF11_ACONFIG_H_

#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/kdconfig/kdconfig.h"
#include "types.h"

struct KDF11_AConfig : public DeviceConfig
{
    KDF11_AConfig ();
    KDF11_AConfig (bool ktf11_aPresent);

    bool ktf11_a_present {false};
    u16 startingAddress {0173000};
    KD11Config::PowerUpMode powerUpMode {KD11Config::PowerUpMode::Bootstrap};
};

#endif // _KDF11_ACONFIG_H_
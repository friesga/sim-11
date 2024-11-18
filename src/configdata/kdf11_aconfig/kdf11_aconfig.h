#ifndef _KDF11_ACONFIG_H_
#define _KDF11_ACONFIG_H_

#include "devicetype.h"
#include "configdata/kd11config/kd11config.h"
#include "types.h"

struct KDF11_AConfig : public DeviceType<BusType::QBus>
{
    KDF11_AConfig ();
    KDF11_AConfig (bool ktf11_aPresent);

    bool ktf11_a_present {false};
    u16 startingAddress {0173000};
    KD11Config::PowerUpMode powerUpMode {KD11Config::PowerUpMode::Bootstrap};
};

#endif // _KDF11_ACONFIG_H_
#ifndef _KDF11_BCONFIG_H_
#define _KDF11_BCONFIG_H_

#include "devicetype.h"
#include "configdata/kd11config/kd11config.h"
#include "configdata/kdf11/slu/sluconfig/sluconfig.h"
#include "configdata/bdv11config/bdv11config.h"

struct KDF11_BConfig : public DeviceType<BusType::QBus>
{
    KD11Config::PowerUpMode powerUpMode {KD11Config::PowerUpMode::Bootstrap};
    SLUConfig sluConfig;
    BDV11Config bdv11Config;
};

#endif // _KDF11_BCONFIG_H_
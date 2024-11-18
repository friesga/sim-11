#ifndef _KD11_NACONFIG_H_
#define _KD11_NACONFIG_H_

#include "devicetype.h"
#include "configdata/kd11config/kd11config.h"

struct KD11_NAConfig : public DeviceType<BusType::QBus>
{
    KD11Config::PowerUpMode powerUpMode {KD11Config::PowerUpMode::Bootstrap};
};

#endif // !_KD11_NACONFIG_H_

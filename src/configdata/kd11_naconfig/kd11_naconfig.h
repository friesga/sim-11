#ifndef _KD11_NACONFIG_H_
#define _KD11_NACONFIG_H_

#include "configdata/kdconfig/kdconfig.h"

struct KD11_NAConfig
{
    KD11Config::PowerUpMode powerUpMode {KD11Config::PowerUpMode::Bootstrap};
};

#endif // !_KD11_NACONFIG_H_

#ifndef _KDF11_BCONFIG_H_
#define _KDF11_BCONFIG_H_

#include "configdata/kd11config/kd11config.h"
#include "../sluconfig/sluconfig.h"
#include "configdata/bdv11config/bdv11config.h"

#include <memory>

using std::shared_ptr;

struct KDF11_BConfig
{
    KD11Config::PowerUpMode powerUpMode {KD11Config::PowerUpMode::Bootstrap};
    shared_ptr<SLUConfig> sluConfig;
    shared_ptr<BDV11Config> bdv11Config;

    KDF11_BConfig ();
};

#endif // _KDF11_BCONFIG_H_
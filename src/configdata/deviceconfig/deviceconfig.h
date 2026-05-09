#ifndef _DEVICECONFIGVARIANT_H_
#define _DEVICECONFIGVARIANT_H_

#include "configdata/ba11/ba11l/ba11lconfig/ba11lconfig.h"
#include "configdata/ba11/ba11n/ba11nconfig/ba11nconfig.h"
#include "configdata/bdv11config/bdv11config.h"
#include "configdata/ka11/ka11config/ka11config.h"
#include "configdata/kd11_naconfig/kd11_naconfig.h"
#include "configdata/kdf11/kdf11_a/kdf11_aconfig/kdf11_aconfig.h"
#include "configdata/kdf11/kdf11_b/kdf11_bconfig/kdf11_bconfig.h"
#include "configdata/kdf11/kdf11_u/kdf11_uconfig/kdf11_uconfig.h"
#include "configdata/kt24/kt24config/kt24config.h"
#include "configdata/m792/m792config/m792config.h"
#include "configdata/m9312/m9312config/m9312config.h"
#include "configdata/msv11config/msv11config.h"
#include "configdata/mm11_econfig/mm11_econfig.h"
#include "configdata/ms11pconfig/ms11pconfig.h"
#include "configdata/rk/rk11d/rk11dconfig/rk11dconfig.h"
#include "configdata/rl/rlconfig/rlconfig.h"
#include "configdata/rxv21/rxv21config/rxv21config.h"
#include "configdata/serialconfig/dl11config/dl11config.h"
#include "configdata/serialconfig/dlv11jconfig/dlv11jconfig.h"
#include "configdata/kdf11/slu/sluconfig/sluconfig.h"

#include <variant>

using std::variant;

using DeviceConfig = variant<
    KD11_NAConfig,
    KDF11_AConfig,
    KDF11_BConfig,
    KT24Config,
    M792Config,
    M9312Config,
    MSV11Config,
    MM11EConfig,
    DL11Config,
    DLV11JConfig,
    BDV11Config,
    RXV21Config,
    RK11DConfig,
    RL11Config,
    RLV11Config,
    RLV12Config,
    BA11_LConfig,
    BA11_NConfig,
    MS11PConfig,
    KDF11_UConfig,
    KA11Config>;

#endif // _DEVICECONFIGVARIANT_H_

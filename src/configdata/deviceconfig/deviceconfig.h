#ifndef _DEVICECONFIGVARIANT_H_
#define _DEVICECONFIGVARIANT_H_

#include "configdata/ba11nconfig/ba11nconfig.h"
#include "configdata/ba11lconfig/ba11lconfig.h"
#include "configdata/bdv11config/bdv11config.h"
#include "configdata/kd11_naconfig/kd11_naconfig.h"
#include "configdata/kdf11/kdf11_a/kdf11_aconfig/kdf11_aconfig.h"
#include "configdata/kdf11/kdf11_b/kdf11_bconfig/kdf11_bconfig.h"
#include "configdata/kdf11/kdf11_u/kdf11_uconfig/kdf11_uconfig.h"
#include "configdata/msv11config/msv11config.h"
#include "configdata/ms11pconfig/ms11pconfig.h"
#include "configdata/rl/rlconfig/rlconfig.h"
#include "configdata/rxv21/rxv21config/rxv21config.h"
#include "configdata/serialconfig/dlv11jconfig/dlv11jconfig.h"
#include "configdata/kdf11/slu/sluconfig/sluconfig.h"

#include <variant>
#include <memory>

using std::variant;
using std::shared_ptr;

using DeviceConfig = variant<
    shared_ptr<KD11_NAConfig>,
    shared_ptr<KDF11_AConfig>,
    shared_ptr<KDF11_BConfig>,
    shared_ptr<MSV11Config>,
    shared_ptr<DLV11JConfig>,
    shared_ptr<BDV11Config>,
    shared_ptr<RXV21Config>,
    shared_ptr<RLV12Config>,
    shared_ptr<BA11_NConfig>,
    shared_ptr<MS11PConfig>,
    shared_ptr<BA11_LConfig>,
    shared_ptr<KDF11_UConfig>>;

#endif // _DEVICECONFIGVARIANT_H_

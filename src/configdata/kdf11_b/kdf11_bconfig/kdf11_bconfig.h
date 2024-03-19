#ifndef _KDF11_BCONFIG_H_
#define _KDF11_BCONFIG_H_

#include "../../kdconfig/kdconfig.h"
#include "../sluconfig/sluconfig.h"
#include "configdata/bdv11config/bdv11config.h"

#include <memory>

using std::shared_ptr;

struct KDF11_BConfig : public KD11Config
{
    shared_ptr<SLUConfig> sluConfig;
    shared_ptr<BDV11Config> bdv11Config;

    KDF11_BConfig ();
};

#endif // _KDF11_BCONFIG_H_
#ifndef _KDF11_BCONFIG_H_
#define _KDF11_BCONFIG_H_

#include "../../kdconfig/kdconfig.h"
#include "../sluconfig/sluconfig.h"

#include <memory>

using std::unique_ptr;
using std::shared_ptr;

struct KDF11_BConfig : public KD11Config
{
    unique_ptr<DeviceConfig> sluConfig;
    shared_ptr<DeviceConfig> bdv11Config;

    KDF11_BConfig ();
};

#endif // _KDF11_BCONFIG_H_
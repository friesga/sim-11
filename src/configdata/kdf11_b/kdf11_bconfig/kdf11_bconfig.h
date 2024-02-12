#ifndef _KDF11_BCONFIG_H_
#define _KDF11_BCONFIG_H_

#include "../../kdconfig/kdconfig.h"
#include "../sluconfig/sluconfig.h"

struct KDF11_BConfig : public KD11Config
{
    SLUConfig sluConfig;

    KDF11_BConfig ();
};

#endif // _KDF11_BCONFIG_H_
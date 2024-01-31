#ifndef _KDF11_ACONFIG_H_
#define _KDF11_ACONFIG_H_

#include "../kd11config/kd11config.h"
#include "types.h"

struct KDF11_AConfig : public KD11Config
{
    KDF11_AConfig ();

    bool ktf11_a_present {false};
    u16 startingAddress {0173000};
};

#endif // _KDF11_ACONFIG_H_
#ifndef _KDF11_ACONFIG_H_
#define _KDF11_ACONFIG_H_

#include "../kd11config/kd11config.h"

struct KDF11_AConfig : public KD11Config
{
    KDF11_AConfig ();

    bool ktf11_a_present {false};
};

#endif // _KDF11_ACONFIG_H_
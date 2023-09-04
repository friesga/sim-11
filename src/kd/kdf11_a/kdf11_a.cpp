#include "kdf11_a.h"

KDF11_A::KDF11_A (Qbus *bus)
    :
    bus_ {bus}
{}


KDF11_A::KDF11_A (Qbus *bus, shared_ptr<KDF11_AConfig> kd11_naConfig)
    :
    KDF11_A (bus)
{}
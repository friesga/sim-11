#include "kdf11_aconfig.h"

KDF11_AConfig::KDF11_AConfig ()
    : 
    KD11Config (DeviceType::KDF11_A)
{}

KDF11_AConfig::KDF11_AConfig (bool ktf11_a)
    : 
    KD11Config (DeviceType::KDF11_A),
    ktf11_a_present {ktf11_a}
{}
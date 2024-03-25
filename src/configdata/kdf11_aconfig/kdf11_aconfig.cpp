#include "kdf11_aconfig.h"

KDF11_AConfig::KDF11_AConfig ()
    : 
    DeviceConfig (DeviceType::KDF11_A)
{}

KDF11_AConfig::KDF11_AConfig (bool ktf11_a)
    : 
    DeviceConfig (DeviceType::KDF11_A),
    ktf11_a_present {ktf11_a}
{}
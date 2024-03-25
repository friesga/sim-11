#include "kdf11_bconfig.h"

using std::make_unique;

KDF11_BConfig::KDF11_BConfig ()
    : 
    DeviceConfig (DeviceType::KDF11_B)
{
    sluConfig = make_unique<SLUConfig> ();
}
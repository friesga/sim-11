#include "kdf11_bconfig.h"

using std::make_unique;

KDF11_BConfig::KDF11_BConfig ()
{
    sluConfig = make_unique<SLUConfig> ();
}
#include "kdf11_uconfig.h"

using std::make_unique;

KDF11_UConfig::KDF11_UConfig ()
{
    sluConfig = make_unique<SLUConfig> ();
}
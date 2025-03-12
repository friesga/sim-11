#include "kdf11_cpucontrol.h"

// The KDF11-U entails a halt flip-flop which is toggled by the ODT Halt
// command. When set the CPU can be single stepped.
//
void KDF11_CpuControl::setHaltMode (bool haltMode)
{
    haltMode_ = haltMode;
}

bool KDF11_CpuControl::inHaltMode ()
{
    return haltMode_;
}

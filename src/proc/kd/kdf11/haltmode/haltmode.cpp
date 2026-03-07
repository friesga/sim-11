#include "haltmode.h"

//
// The KDF11-U entails a halt flip-flop which is toggled by the ODT Halt
// command. When set the CPU can be single stepped.
//
void KDF11_HaltMode::setHaltMode (bool haltMode)
{
    haltMode_ = haltMode;
}

bool KDF11_HaltMode::inHaltMode () const
{
    return haltMode_;
}

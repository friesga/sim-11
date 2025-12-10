#include "pseudo_haltmode.h"

//
// The HaltMode is only implemented in the KDF11-U. inHaltMode() returns
// false to prevent the CPU being halted by an unsupported feature.
// 
void PseudoHaltMode::setHaltMode (bool haltMode)
{}

bool PseudoHaltMode::inHaltMode () const
{
    return false;
}

#include "dummy_haltmode.h"

//
// The HaltMode is only implemented in the KDF11-U. inHaltMode() returns
// false to prevent the CPU being halted by an unsupported feature.
// 
void DummyHaltMode::setHaltMode (bool haltMode)
{}

bool DummyHaltMode::inHaltMode () const
{
    return false;
}

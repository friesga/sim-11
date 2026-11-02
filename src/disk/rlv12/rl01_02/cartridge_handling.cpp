#include "rl01_02.h"

// This file contains the functions required by the diskdrive interface
// to facilitate the handling (i.e. the loading and unloading) of cartridges
// for the diskdrive.

void RL01_02::unloadCartridge ()
{ }

void RL01_02::loadCartridge (Cartridge)
{ }

bool RL01_02::cartridgeIsLoaded ()
{
    return false;
}

bool RL01_02::cartridgeCanBeLoaded ()
{
    return stateMachine_->inState (SpindleStopped {});
}
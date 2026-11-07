#include "rl01_02.h"

using std::string;

// This file contains the functions required by the diskdrive interface
// to facilitate the handling (i.e. the loading and unloading) of cartridges
// for the diskdrive.

bool RL01_02::unloadCartridge ()
{
    if (!stateMachine_->inState (SpindleStopped {}))
        return false;

    cartridge_ = nullptr;
    stateMachine_->dispatch (UnloadedCartridge {});
    return true;
}

bool RL01_02::loadCartridge (string fileName)
{
    if (fileName.empty () 
            || !(stateMachine_->inState (CartridgeUnloaded {})
            || stateMachine_->inState (SpindleStopped {})))
        return false;

    cartridge_ = Cartridge::create (fileName, geometry_,
        Cartridge::OpenMode::ReadWrite);

    if (cartridge_ == nullptr)
        return false;
    
    stateMachine_->dispatch (LoadedCartridge {});

    return true;
}

bool RL01_02::spindleStopped ()
{
    return stateMachine_->inState (SpindleStopped {});
}

bool RL01_02::noCartridgeLoaded ()
{
    return stateMachine_->inState (CartridgeUnloaded {});
}
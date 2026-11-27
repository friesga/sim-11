#include "rl01_02.h"

using std::string;

// This file contains the functions required by the diskdrive interface
// to facilitate the handling (i.e. the loading and unloading) of cartridges
// for the diskdrive.

bool RL01_02::unloadCartridge ()
{
    if (driveState () != DiskDrive::State::CartridgeLoaded_SpindleStopped)
        return false;

    cartridge_ = nullptr;
    stateMachine_->dispatch (UnloadedCartridge {});
    return true;
}

bool RL01_02::loadCartridge (string fileName)
{
    if (driveState () != DiskDrive::State::NoCartridgeLoaded)
        return false;

    cartridge_ = Cartridge::create (fileName, geometry_,
        Cartridge::OpenMode::ReadWrite);

    if (cartridge_ == nullptr)
        return false;
    
    stateMachine_->dispatch (LoadedCartridge {});
    return true;
}

DiskDrive::State RL01_02::driveState ()
{
    return driveStateMapping_[static_cast<size_t> (stateMachine_->currentState ().index ())];
}

bool RL01_02::spindleStopped ()
{
    return stateMachine_->inState (SpindleStopped {});
}

bool RL01_02::noCartridgeLoaded ()
{
    return stateMachine_->inState (CartridgeUnloaded {});
}

string RL01_02::cartridgeFileName () const
{
    return (cartridge_ != nullptr) ? cartridge_->attachedFileName () : "";
}
#include "rk05.h"

#include <array>

using std::string;
using std::array;

// This file contains the functions required by the diskdrive interface
// to facilitate the handling (i.e. the loading and unloading) of cartridges
// for the diskdrive.

bool RK05::unloadCartridge ()
{
    if (driveState () != DiskDrive::State::CartridgeLoaded_SpindleStopped)
        return false;

    cartridge_ = nullptr;
    driveSystem_->show (Front::State::Primary);
    stateMachine_->dispatch (UnloadedCartridge {});
    return true;
}

bool RK05::loadCartridge (string fileName)
{
    if (driveState () != DiskDrive::State::NoCartridgeLoaded)
        return false;
    
    driveSystem_->show (Front::State::Secondary);
    stateMachine_->dispatch (LoadedCartridge {});
    return true;
}

DiskDrive::State RK05::driveState ()
{
    return driveStateMapping_[static_cast<size_t> (stateMachine_->currentState ().index ())];
}

bool RK05::spindleStopped ()
{
    return stateMachine_->inState (SpindleStopped {});
}

bool RK05::noCartridgeLoaded ()
{
    return stateMachine_->inState (CartridgeUnloaded {});
}

string RK05::cartridgeFileName () const
{
    return (cartridge_ != nullptr) ? cartridge_->attachedFileName () : "";
}
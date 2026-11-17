#include "rk05.h"

using std::string;

// This file contains the functions required by the diskdrive interface
// to facilitate the handling (i.e. the loading and unloading) of cartridges
// for the diskdrive.

bool RK05::unloadCartridge ()
{
/*
    if (!stateMachine_->inState (SpindleStopped {}))
        return false;

    cartridge_ = nullptr;
    stateMachine_->dispatch (UnloadedCartridge {});
*/
    return true;
}

bool RK05::loadCartridge (string fileName)
{
/*
    if (fileName.empty () 
            || !(stateMachine_->inState (CartridgeUnloaded {})
            || stateMachine_->inState (SpindleStopped {})))
        return false;

    cartridge_ = Cartridge::create (fileName, geometry_,
        Cartridge::OpenMode::ReadWrite);

    if (cartridge_ == nullptr)
        return false;
    
    stateMachine_->dispatch (LoadedCartridge {});
*/
    return true;
}

bool RK05::spindleStopped ()
{
    // return stateMachine_->inState (SpindleStopped {});
    return false;
}

bool RK05::noCartridgeLoaded ()
{
    // return stateMachine_->inState (CartridgeUnloaded {});
    return false;
}

string RK05::cartridgeFileName () const
{
    // return (cartridge_ != nullptr) ? cartridge_->attachedFileName () : "";
    return "";
}
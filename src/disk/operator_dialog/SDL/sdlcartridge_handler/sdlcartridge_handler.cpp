#include "sdlcartridge_handler.h"

#include <iostream>

SDLCartridgeHandler::SDLCartridgeHandler (DiskDrive* diskDrive)
    :
    diskDrive_ {diskDrive}
{}

void SDLCartridgeHandler::operatorActionRequested (int x, int y)
{
     OperatorAction action = getOperatorAction (x, y);

    switch (action)
    {
        case OperatorAction::UnloadCartridge:
            unloadCartridge ();
            break;
            
        case OperatorAction::LoadCartridge:
            loadCartridge ();
            break;

        case OperatorAction::DownloadCartridge:
            // Ignore, should not happen
            [[fallthrough]];

        case OperatorAction::None:
            [[fallthrough]];

        default:
            break;
    }
}

// The function getOperatorAction returns the action requested by the operator.
//
const CartridgeHandler::OperatorAction SDLCartridgeHandler::getOperatorAction (int x, int y)
{
    // Open the popup and get the result (if any)
    const MenuOption* action = popupMenu_.getOption (x, y);

    return (action == nullptr) ? OperatorAction::None :
        static_cast<OperatorAction> (action->value ());
}

void SDLCartridgeHandler::unloadCartridge ()
{
    switch (diskDrive_->driveState ())
    {
        case DiskDrive::State::CartridgeLoaded_SpindleStopped:
            if (diskDrive_->unloadCartridge ())
            {
                if (diskDrive_->requiresConfirmation ())
                    messageBox_.show ("Cartridge has been unloaded");
            }
            else
                messageBox_.show ("Cartridge could not be unloaded");
            break;

        case DiskDrive::State::NoPower:
             messageBox_.show ("Apply power before the cartridge can be unloaded");
             break;

        case DiskDrive::State::NoCartridgeLoaded:
             messageBox_.show ("No cartridge to be unloaded");
             break;

        case DiskDrive::State::CartridgeLoaded_DriveSpunUp:
             messageBox_.show ("Spin down disk before unloading the cartridge");
             break;

        default:
            messageBox_.show ("Unknown error on unloading of cartridge");
            break;
    }
}

void SDLCartridgeHandler::loadCartridge ()
{
    switch (diskDrive_->driveState ())
    {
        case DiskDrive::State::NoCartridgeLoaded:
        {
            SDLFileDialog::DialogResult dialogResult =
                fileDialog_.loadFileDialog ();

            if (dialogResult)
            {
                diskDrive_->loadCartridge (dialogResult.value ());

                if (diskDrive_->requiresConfirmation ())
                    messageBox_.show ("Cartridge has been loaded");
            }
            else
                messageBox_.show ("Cartridge could not be loaded");
            break;
        }

        case DiskDrive::State::NoPower:
             messageBox_.show ("Apply power before the cartridge can be loaded");
             break;

        case DiskDrive::State::CartridgeLoaded_SpindleStopped:
             messageBox_.show ("Unload cartridge before loading another cartridge");
             break;

        case DiskDrive::State::CartridgeLoaded_DriveSpunUp:
             messageBox_.show ("Spin down and unload cartridge before loading another cartridge");
             break;

        default:
            messageBox_.show ("Unknown error on loading of cartridge");
            break;
    }
}
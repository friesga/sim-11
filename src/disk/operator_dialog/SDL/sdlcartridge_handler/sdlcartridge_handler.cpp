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
    if (diskDrive_->spindleStopped ())
    {
        diskDrive_->unloadCartridge ();
        messageBox_.show ("Cartridge has been unloaded");
    }
    else if (diskDrive_->noCartridgeLoaded ())
        messageBox_.show ("No cartridge loaded");
    else
        messageBox_.show ("Cartridge cannot be unloaded");
}

void SDLCartridgeHandler::loadCartridge ()
{
    if (diskDrive_->spindleStopped () || diskDrive_->noCartridgeLoaded ())
    {
        SDLFileDialog::DialogResult dialogResult =
            fileDialog_.loadFileDialog ();

        if (dialogResult)
        {
            diskDrive_->loadCartridge (dialogResult.value ());
            messageBox_.show ("Cartridge has been loaded");
        }
        else
            messageBox_.show ("Cartridge could not be loaded");
    }
    else
        messageBox_.show ("Spin down disk before loading a cartridge ");
}
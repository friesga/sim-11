#include "sdlcartridge_handler.h"

#include <iostream>

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
    messageBox_.show ("Cartridge unloaded");
}

void SDLCartridgeHandler::loadCartridge ()
{
    SDLFileDialog::DialogResult dialogResult = fileDialog_.loadFileDialog ();
    if (dialogResult)
        std::cerr << "Selected file: " << dialogResult.value () << '\n';
    else
        std::cerr << "No file selected\n";
}
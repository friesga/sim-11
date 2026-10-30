#ifndef _SDL_CARTRIDGE_HANDLER_H_
#define _SDL_CARTRIDGE_HANDLER_H_

#include "disk/operator_dialog/include/cartridge_handler.h"
#include "../../popupmenu/menuoptionlist/menuoptionlistimpl.h"
#include "../../popupmenu/menuoption/menuoptionimpl.h"
#include "../sdlpopupmenu/sdlpopupmenu.h"
#include "../sdlmessagebox/sdlmessagebox.h"
#include "../sdlfiledialog/sdlfiledialog.h"

// As its name suggests the class CartridgeHandler is responsible for handling
// the cartridge operations in the emulator. It provides methods to load and
// unload a cartridge.
//
// For the web environment loading a cartridge implies uploading the cartridge
// image data to the MEMFS and unloading the cartridge offers the possibility
// to download the cartridge image data from the MEMFS.
//
class SDLCartridgeHandler : public CartridgeHandler
{
public:
    ~SDLCartridgeHandler () = default;
    void operatorActionRequested (int x, int y) override;

private:
    using OperatorActionOption = MenuOptionImpl<OperatorAction>;

    const MenuOptionListImpl<OperatorActionOption> operatorActionMenu
    {
        OperatorActionOption {OperatorAction::UnloadCartridge,   "Unload Cartridge"},
        OperatorActionOption {OperatorAction::LoadCartridge,   "Load Cartridge"}
    };

     SDLPopupMenu popupMenu_ {operatorActionMenu};
     SDLMessageBox messageBox_;
     SDLFileDialog fileDialog_;

    const OperatorAction getOperatorAction (int x, int y);
    void unloadCartridge ();
    void loadCartridge ();
};

#endif // _SDL_CARTRIDGE_HANDLER_H_
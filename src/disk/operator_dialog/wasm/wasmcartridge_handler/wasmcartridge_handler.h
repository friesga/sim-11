#ifndef _WASM_CARTRIDGE_HANDLER_H_
#define _WASM_CARTRIDGE_HANDLER_H_

#include "disk/operator_dialog/include/cartridge_handler.h"
#include "../../popupmenu/menuoptionlist/menuoptionlistimpl.h"
#include "../../popupmenu/menuoption/menuoptionimpl.h"
#include "../wasmpopupmenu/wasmpopupmenu.h"
#include "../wasmmessagebox/wasmmessagebox.h"
#include "../wasmfiledialog/wasmfiledialog.h"
#include "diskdrive.h"

// As its name suggests the class CartridgeHandler is responsible for handling
// the cartridge operations in the emulator. It provides methods to load and
// unload a cartridge.
//
// For the web environment loading a cartridge implies uploading the cartridge
// image data to the MEMFS and unloading the cartridge offers the possibility
// to download the cartridge image data from the MEMFS.
//
class WASMCartridgeHandler : public CartridgeHandler
{
public:
    WASMCartridgeHandler (DiskDrive* diskDrive);
    ~WASMCartridgeHandler () = default;
    void operatorActionRequested (int x, int y) override;
    void menuItemSelected (const MenuOption* menuOption);
    

private:
    using OperatorActionOption = MenuOptionImpl<OperatorAction>;

    const MenuOptionListImpl<OperatorActionOption> operatorActionMenu
    {
        OperatorActionOption {OperatorAction::UnloadCartridge,   "Unload Cartridge"},
        OperatorActionOption {OperatorAction::DownloadCartridge, "Download Cartridge"},
        OperatorActionOption {OperatorAction::LoadCartridge,     "Load Cartridge"}
    };

    WASMPopupMenu popupMenu_ {operatorActionMenu,
        [this] (const MenuOption* menuOption) {this->menuItemSelected (menuOption); }};
    WASMMessageBox messageBox_ {[this] {return; }};
    WASMFileDialog dialog_;
    DiskDrive* diskDrive_;
    std::string uploadedFileName_;

    void loadCartridge ();
    void unloadCartridge ();
    void downloadCartridge ();
    void fileSelected (WASMFileDialog::File* selectedFile);
    void uploadCompleted (bool success);
    void downloadCompleted (WASMFileDialog::DownloadResult result);
    bool fileExists (const std::string& fileName);
};

#endif // _WASM_CARTRIDGE_HANDLER_H_
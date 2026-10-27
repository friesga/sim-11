#ifndef _CARTRIDGE_HANDLER_H_
#define _CARTRIDGE_HANDLER_H_


// As its name suggests the class CartridgeHandler is responsible for handling
// the cartridge operations in the emulator. It provides methods to load and
// unload a cartridge.
//
class CartridgeHandler
{
public:
    enum class OperatorAction
    {
        UnloadCartridge,
        DownloadCartridge,
        LoadCartridge,
        None
    };

    virtual ~CartridgeHandler () = default;
    virtual void operatorActionRequested (int x, int y) = 0;
};


#endif // _CARTRIDGE_HANDLER_H_
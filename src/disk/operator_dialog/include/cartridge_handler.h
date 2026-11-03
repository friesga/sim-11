#ifndef _CARTRIDGE_HANDLER_H_
#define _CARTRIDGE_HANDLER_H_

#include "diskdrive.h"

#include <functional>
#include <memory>

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

 // A CartridgeHandlerFactory is a function that will create a
 // CartridgeHandler and return a unique_ptr to it. The factory is passed
 // to the disk drives which will call the factory to create the
 // CartridgeHandler. The handler varies depending on the platform.
using CartridgeHandlerFactory =
    std::function<std::unique_ptr<CartridgeHandler> (DiskDrive*)>;

#endif // _CARTRIDGE_HANDLER_H_
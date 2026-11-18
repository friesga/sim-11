#include "disk/operator_dialog/include/cartridge_handler.h"

class FakeCartridgeHandler : public CartridgeHandler
{
public:
    explicit FakeCartridgeHandler (DiskDrive* diskDrive) {};
    ~FakeCartridgeHandler () = default;
    void operatorActionRequested (int x, int y) override {};
};
#ifndef _REGISTERHANDLER_H_
#define _REGISTERHANDLER_H_

#include "qbus/qbus.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "kd/common/controllogic/controllogic.h"
#include "kd/kdf11/odt/kdf11_odt.h"

#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

class RegisterHandler : public AbstractBusDevice
{
public:
    RegisterHandler (vector<BusDevice*> cpuModules);

    // Functions required by the BusDevice interface
    // The function writeByte() is implemented by AbstractBusDevice
    StatusCode read (BusAddress<> address, u16* destination) override;
    StatusCode writeWord (BusAddress<> address, u16 value) override;
    bool responsible (BusAddress<> address) override;
    void reset () override;

private:
    // The cpuModules_ vector contains the modules having registers that
    // accessible from the bus. The CpuData's registers (i.e. just the PSW)
    // will be present. The KTF11-A will be added when it's defined in the
    // configuration.
    vector<BusDevice*> cpuModules_ {};
};

#endif // _REGISTERHANDLER_H_
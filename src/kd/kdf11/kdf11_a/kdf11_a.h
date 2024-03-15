#ifndef _KDF11_A_H_
#define _KDF11_A_H_

#include "kd/include/pdp11processor.h"
#include "kd/kdf11/cpucontrol/kdf11_cpucontrol.h"
#include "kd/kdf11/cpudata/kdf11cpudata.h"
#include "kd/kdf11/odt/kdf11_odt.h"
#include "configdata/kdf11_aconfig/kdf11_aconfig.h"
#include "kd/common/controllogic/controllogic.h"
#include "kd/kdf11/registeraccess/registeraccess.h"

#include <memory>
#include <vector>
#include <thread>

using std::unique_ptr;
using std::shared_ptr;
using std::vector;
using std::thread;

// The following versions of the KDF11-A exist:
// 1. KDF11-AA processor with memory management unit (MMU),
// 2. KDF11-AB processor with MMU and floating point,
// 3. KDF11-AC processor without MMU or floating point.
// 
// This class implements the KDF11-AA and KDF11-AC versions. The presence of
// the MMU is configurable. The floating point processor currently isn't
// supported.
// 
// The class KDF11_A starts the control logic which on its turn has to run
// the KDF11_A's cpu and start the KD11_FA's ODT.
//
class KDF11_A : public PDP11Processor
{
public:
    KDF11_A (Qbus* bus, shared_ptr<KDF11_AConfig> kdf11_aConfig);
    ~KDF11_A ();

    // Functions required by the BusDevice interface. The function writeByte()
    // is implemented in AbstractBusDevice.
    StatusCode read (BusAddress address, u16* destination) override;
    StatusCode writeWord (BusAddress address, u16 value) override;
    bool responsible (BusAddress address) override;
    void reset () override;

    // Functions required by the PDP11Processor interface.
    void start (u16 startAddress);
    void start ();

private:
    Qbus* bus_;

    // Definition of the KDF11-A components. The KTF11-A (MMU) is optional.
    KDF11CpuData cpuData_ {};
    KTF11_A mmu_ {bus_, &cpuData_};
    KDF11_CpuControl cpuControl_ {bus_, &cpuData_, &mmu_};
    unique_ptr<ControlLogic> controlLogic_;

    // RegisterAccess performs the functions required by the BusDevice
    // interface. These functions are put in a separate class as they are used
    // by both the KDF11_A and KDF11_B classes.
    unique_ptr<RegisterAccess> registerAccess_;

    // The KDF11_A is started in its own thread
    thread kd11Thread_;

    u16 startAddress_;
};

#endif // !_KDF11_A_H_

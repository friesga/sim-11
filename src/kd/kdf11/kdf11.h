#ifndef _KDF11_H_
#define _KDF11_H_

#include "kd/include/pdp11processor.h"
#include "kd/kdf11/cpucontrol/kdf11_cpucontrol.h"
#include "kd/kdf11/odt/kdf11_a_odt.h"
#include "configdata/kd11config/kd11config.h"
#include "kd/common/controllogic/controllogic.h"
#include "kd/kdf11/cpudata/kdf11cpudata.h"

// The class KDF11 is a super class for the KDF11_A and KDF11-B classes,
// harbouring the common classes for these processors.
class KDF11 : public PDP11Processor
{
public:
    KDF11 (Qbus* bus, KD11Config::PowerUpMode powerUpMode, u16 startAddress);
    ~KDF11 ();
    void start (u16 startAddress);
    void start ();

    // Give unit tests access to the CPU, CpuData and the MMU.
    constexpr CpuControl* cpuControl ();
    constexpr CpuData* cpuData ();
    constexpr MMU* mmu ();

    // The KDF11_A is a PDP11Peripheral with at least one register, the
    // PSW.
    // The function writeByte() is implemented in PDP11Peripheral
    StatusCode read (BusAddress address, u16* destination) override;
    StatusCode writeWord (BusAddress address, u16 value) override;
    bool responsible (BusAddress address) override;
    void reset () override;

protected:
    enum { stdBootAddress = 0173000 };

    Qbus* bus_;
    KDF11_ACpuData cpuData_ {};
    KTF11_A mmu_ {bus_, &cpuData_};
    KDF11_CpuControl cpuControl_ {bus_, &cpuData_, &mmu_};
    unique_ptr<KDF11_A_ODT>	odt_ {};
    KD11Config::PowerUpMode powerUpMode_;
    u16 startAddress_;
    unique_ptr<ControlLogic> controlLogic_;

    // The cpuModules_ vector contains the modules having registers that
    // accessible from the bus. The CpuData's registers (i.e. just the PSW)
    // will be present. The KTF11-A will be added when it's defined in the
    // configuration.
    vector<BusDevice*> cpuModules_ {&cpuData_};

    // The KDF11_A is started in its own thread
    std::thread kd11Thread_;
};

constexpr CpuControl* KDF11::cpuControl ()
{
    return &cpuControl_;
}

constexpr CpuData* KDF11::cpuData ()
{
    return &cpuData_;
}

constexpr MMU* KDF11::mmu ()
{
    return &mmu_;
}

#endif // _KDF11_H_
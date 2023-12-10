#ifndef _KDF11_A_H_
#define _KDF11_A_H_

#include "kd/include/pdp11processor.h"
#include "kd/kdf11_a/cpucontrol/kdf11_a_cpucontrol.h"
#include "kd/kd11_na/odt/kd11_na_odt.h"
#include "configdata/kdf11_aconfig/kdf11_aconfig.h"
#include "kd/common/controllogic/controllogic.h"
#include "kd/kdf11_a/cpudata/kdf11_acpudata.h"

#include <memory>

using std::unique_ptr;
using std::shared_ptr;

// The class KDF11_A starts the control logic which on its turn has to run
// the KDF11_A's cpu and start the KD11_FA's ODT.
class KDF11_A : public PDP11Processor
{
public:
    KDF11_A (Qbus* bus);
    KDF11_A (Qbus* bus, shared_ptr<KDF11_AConfig> kdf11_aConfig);
    ~KDF11_A ();
    void start (u16 startAddress);
    void start ();

    // Give unit tests access to the CPU, CpuData and the MMU.
    constexpr CpuControl* cpuControl ();
    constexpr CpuData* cpuData ();
    constexpr MMU* mmu ();

    // The KDF11_A is a PDP11Peripheral with at least one register, the
    // PSW.
    // The function writeByte() is implemented in PDP11Peripheral
    StatusCode read (u16 address, u16* destination) override;
    StatusCode writeWord (u16 address, u16 value) override;
    bool responsible (u16 address) override;
    void reset () override {};

private:
    enum { stdBootAddress = 0173000 };
    enum { PSWAddress = 0177776 };

    Qbus* bus_;
    KDF11_ACpuData cpuData_ {};
    KTF11_A mmu_ {bus_, &cpuData_};
    KDF11_A_CpuControl cpuControl_ {bus_, &cpuData_, &mmu_};
    unique_ptr<KD11_NA_ODT>	odt_ {};
    KDF11_AConfig::PowerUpMode powerUpMode_;
    u16 startAddress_;
    unique_ptr<ControlLogic> controlLogic_;

    // The KDF11_A is started in its own thread
    std::thread kd11Thread_;
};

constexpr CpuControl* KDF11_A::cpuControl ()
{
    return &cpuControl_;
}

constexpr CpuData* KDF11_A::cpuData ()
{
    return &cpuData_;
}

constexpr MMU* KDF11_A::mmu ()
{
    return &mmu_;
}

#endif // !_KDF11_A_H_

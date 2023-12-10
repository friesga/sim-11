#ifndef _KD11_NA_H_
#define _KD11_NA_H_

#include "kd/include/pdp11processor.h"
#include "cpucontrol/kd11_na_cpucontrol.h"
#include "odt/kd11_na_odt.h"
#include "kd/kd11_na/cpudata/kd11_nacpudata.h"
#include "kd/kd11_na/pseudommu/pseudommu.h"
#include "configdata/kd11_naconfig/kd11_naconfig.h"
#include "kd/common/controllogic/controllogic.h"

#include <memory>

using std::unique_ptr;
using std::shared_ptr;

// The class KD11_NA starta the control logic which on its turn has to run
// the KD11_NA's cpu and start the KD11_NA's ODT.
class KD11_NA : public PDP11Processor
{
public:
    KD11_NA (Qbus* bus);
    KD11_NA (Qbus* bus, shared_ptr<KD11_NAConfig> kd11_naConfig);
    ~KD11_NA ();
    void start (u16 startAddress);
    void start ();

    // Give unit tests access to the CPU, CpuData and the MMU.
    constexpr CpuControl* cpuControl ();
    constexpr CpuData* cpuData ();
    constexpr MMU* mmu ();

    // The KD11_NA is a PDP11Peripheral without registers so the read and write 
    // register functions are dummies. The reset function is called on a
    // bus reset and has no function for the KD11_NA either.
    StatusCode read (u16 addr, u16* destination) override
        { return StatusCode::FunctionNotImplemented; };
    StatusCode writeWord (u16 addr, u16 value) override
        { return StatusCode::FunctionNotImplemented; };
    StatusCode writeByte (u16 addr, u8 value) override
        { return StatusCode::FunctionNotImplemented; };
    bool responsible (u16 addr) override
        { return false; }
    void reset () override {};

private:
    enum { stdBootAddress = 0173000 };

    Qbus* bus_;
    KD11_NACpuData cpuData_;
    KD11_NA_CpuControl cpuControl_ {bus_, &cpuData_, &pseudoMMU_};
    PseudoMMU pseudoMMU_ {bus_, &cpuData_};
    unique_ptr<KD11_NA_ODT>	odt_ {};
    KD11_NAConfig::PowerUpMode powerUpMode_;
    u16 startAddress_;
    unique_ptr<ControlLogic> controlLogic_;

    // The KD11_NA is started in its own thread
    std::thread kd11Thread_;
};

constexpr CpuControl* KD11_NA::cpuControl ()
{
    return &cpuControl_;
}

constexpr CpuData* KD11_NA::cpuData ()
{
    return &cpuData_;
}

constexpr MMU* KD11_NA::mmu ()
{
    return &pseudoMMU_;
}


#endif // !_KD11_NA_H_

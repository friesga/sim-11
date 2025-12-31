#ifndef _KD11_NA_H_
#define _KD11_NA_H_

#include "bus/include/bus.h"
#include "proc/kd/include/pdp11processor.h"
#include "proc/common/composite_cpucontroller/composite_cpucontroller.h"
#include "odt/kd11_na_odt.h"
#include "proc/kd/kd11_na/cpudata/kd11_nacpudata.h"
#include "proc/common/pseudommu/pseudommu.h"
#include "configdata/kd11_naconfig/kd11_naconfig.h"
#include "proc/kd/common/kdmachinestate/kdmachinestate.h"
#include "proc/kd/kd11_na/executor/executor.h"
#include "proc/kd/kd11_na/calculate/calculate.h"
#include "proc/common/pseudo_haltmode/pseudo_haltmode.h"
#include "proc/common/basicprocessorexception/basicprocessorexception.h"

#include <memory>

using std::unique_ptr;

// Two different LSI-models exist, the LSI-11 and the LSI-11/2. The LSI-11
// comprises the M7264 module in one of its variations. The LSI-11/2
// consists of a M7270 module with a KD11-HA or KD11-NA processor. These
// processors differ in the availability of the EIS and FIS options.
// See http://web.frainresearch.org:8080/projects/pdp-11/lsi-11.php for
// an overview of the different variations. 
// This class simulates a KD11-NA, i.e. a KD11-H base version including EIS
// and FIS support.
//
// The class KD11_NA starts CPU controller which on its turn has to run
// the KD11_NA's CPU and start the KD11_NA's ODT.
//
class KD11_NA : public PDP11Processor
{
public:
    KD11_NA (Bus* bus);
    KD11_NA (Bus* bus, const KD11_NAConfig& kd11_naConfig);
    ~KD11_NA ();
    void start (u16 startAddress);
    void start ();

    // Give unit tests access to the CPU, CpuData and the MMU.
    constexpr CpuControl* cpuControl ();
    constexpr CpuData* cpuData ();
    constexpr MMU* mmu ();

    // The KD11_NA is a peripheral without registers so the read and write 
    // register functions are dummies. The reset function is called on a
    // bus reset and has no function for the KD11_NA either.
    CondData<u16> read (BusAddress address) override
        { return {StatusCode::FunctionNotImplemented}; };
    StatusCode writeWord (BusAddress address, u16 value) override
        { return StatusCode::FunctionNotImplemented; };
    StatusCode writeByte (BusAddress address, u8 value) override
        { return StatusCode::FunctionNotImplemented; };
    bool responsible (BusAddress address) override
        { return false; }
    void reset () override {};

private:
    enum { stdBootAddress = 0173000 };

    Bus* bus_;
    KD11_NACpuData cpuData_;
    CompositeCpuController<KD11_NA_Executor, KD11_NA_Calculate,
        PseudoHaltMode, BasicProcessorException> cpuControl_ {bus_, &cpuData_, &pseudoMMU_};
    PseudoMMU pseudoMMU_ {bus_, &cpuData_};
    unique_ptr<KD11_NA_ODT>	odt_ {};
    KD11Config::PowerUpMode powerUpMode_;
    u16 startAddress_;
    unique_ptr<KDMachineState> machineState_;

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

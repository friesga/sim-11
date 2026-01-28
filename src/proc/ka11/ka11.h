#ifndef _KA11_H_
#define _KA11_H_

#include "bus/include/bus.h"
#include "proc/kd/include/pdp11processor.h"
#include "configdata/ka11/ka11config/ka11config.h"
#include "devicecommon/registerhandler/registerhandler.h"
#include "proc/ka11/ky11_a/ky11_a.h"
#include "proc/ka11/ka11machinestate/ka11machinestate.h"
#include "proc/ka11/ka11cpudata/ka11cpudata.h"
#include "proc/common/pseudoMMU/pseudommu.h"
#include "proc/common/composite_cpucontroller/composite_cpucontroller.h"
#include "proc/ka11/executor/executor.h"
#include "proc/ka11/calculator/calculator.h"
#include "proc/common/pseudo_haltmode/pseudo_haltmode.h"
#include "proc/common/basicprocessorexceptionhandler/basicprocessorexceptionhandler.h"
#include "panel.h"

#include <memory>

using std::unique_ptr;

// The KA11 is the CPU of the PDP-11/20.
//
class KA11 : public PDP11Processor
{
public:
    KA11 (Bus* bus, Window* window, const KA11Config& ka11Config);
    ~KA11 ();

    // Functions required by the BusDevice interface
    CondData<u16> read (BusAddress address);
    StatusCode writeWord (BusAddress address, u16 value);
    bool responsible (BusAddress address);
    void reset ();

    // Functions required by the PDP11Processor interface.
    void start (u16 startAddress);
    void start ();

private:
    Bus* bus_;

    // Definition of the KA11 components.
    KA11CpuData cpuData_ {};
    PseudoMMU mmu_ {bus_, &cpuData_};
    CompositeCpuController<KA11_Executor, KA11Calculator,
        PseudoHaltMode, BasicProcessorExceptionHandler> cpuController_ {bus_, &cpuData_, &mmu_};
    unique_ptr<KY11_A> ky11_a_;
    unique_ptr<KA11MachineState> machineState_;
    u16 startAddress_ {0};

    // RegisterHandler performs the functions required by the BusDevice
    // interface. These functions are put in a separate class as they are
    // also used by the KDF11_A and KDF11_B classes.
    unique_ptr<RegisterHandler> registerHandler_;

    // The KA11 is started in its own thread
    std::thread ka11Thread_;
};

#endif _KA11_H_
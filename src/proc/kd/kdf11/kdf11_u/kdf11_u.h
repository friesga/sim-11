#ifndef _KDF11_U_H_
#define _KDF11_U_H_

#include "proc/kd/include/pdp11processor.h"
#include "proc/kd/kdf11/odt/kdf11_odt.h"
#include "configdata/kdf11/kdf11_u/kdf11_uconfig/kdf11_uconfig.h"
#include "proc/kd/common/kdmachinestate/kdmachinestate.h"
#include "proc/kd/kdf11/cpudata/kdf11cpudata.h"
#include "devicecommon/registerhandler/registerhandler.h"
#include "proc/kd/kdf11/serial_line_units/serial_line_units.h"
#include "proc/kd/kdf11/kdf11_u/displayregister/displayregister.h"
#include "proc/common/composite_cpucontroller/composite_cpucontroller.h"
#include "proc/kd/kdf11/executor/executor.h"
#include "proc/kd/kdf11/calculate/calculate.h"
#include "proc/kd/kdf11/haltmode/haltmode.h"
#include "proc/kd/kdf11/kdf11_executionengine/kdf11_executionengine.h"
#include "proc/kd/kdf11/kdf11processorexception/kdf11processorexception.h"

#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

// The KDF11-UA is a module which contains the following features:
// - Central Processor Unit,
// - Memory Management Unit (MMU),
// - Floating-Point Processor (FPP) Option,
// - Commercial Instruction Set Processor (CISP) Option,
// - Two Serial Line Interfaces (SLU),
// - Line Frequency Interrupt Real Time Clock,
// - Extended UNIBUS (EUB).
//
// Note the KDF11-U has, unlike the KDF11-B, no BDV11 incorporated. Boot
// ROMS are provided by either a M9312 bootstrap/terminator module or the
// optional Unibus map module (M7134).
//
// As the the KDF11-U contains no BDV11 it also doesn't contain the BDV11's
// display register. This register is provided as 11/70 compatible register
// at address 0177570.
//
class KDF11_U : public PDP11Processor
{
public:
    KDF11_U (Bus* bus, const KDF11_UConfig& kdf11_uConfig);
    ~KDF11_U ();
    
    // Functions required by the BusDevice interface. The function writeByte()
    // is implemented in AbstractBusDevice.
    CondData<u16> read (BusAddress address) override;
    StatusCode writeWord (BusAddress address, u16 value) override;
    bool responsible (BusAddress address) override;
    void reset () override;

    // Functions required by the PDP11Processor interface.
    void start (u16 startAddress);
    void start ();

private:
    enum { stdBootAddress = 0173000 };

    Bus* bus_;

    // Definition of the KDF11-U components
    KDF11CpuData cpuData_ {};
    KTF11_A mmu_ {bus_, &cpuData_};
    CompositeCpuController<KDF11_Executor, KDF11_Calculate,
        KDF11_HaltMode, ExecutionEngine<KDF11_Executor, KDF11_Calculate, KDF11ProcessorException>,
        KDF11ProcessorException> cpuControl_ {bus_, &cpuData_, &mmu_};
    unique_ptr<KDMachineState> machineState_;
    unique_ptr<SerialLineUnits> serialLineUnits;
    DisplayRegister displayRegister_;

    // RegisterHandler performs the functions required by the BusDevice
    // interface. These functions are put in a separate class as they are
    // also used by the KDF11_A and KDF11_B classes.
    unique_ptr<RegisterHandler> registerHandler_;

    // The KDF11_U is started in its own thread
    thread kd11Thread_ {};

    u16 startAddress_;
};

#endif // !_KDF11_U_H_

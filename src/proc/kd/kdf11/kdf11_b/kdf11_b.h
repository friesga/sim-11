#ifndef _KDF11_B_H_
#define _KDF11_B_H_

#include "proc/kd/include/pdp11processor.h"
#include "proc/kd/kdf11/odt/kdf11_odt.h"
#include "proc/kd/kdf11/ktf11_a/ktf11_a.h"
#include "configdata/kdf11/kdf11_b/kdf11_bconfig/kdf11_bconfig.h"
#include "proc/kd/common/kdmachinestate/kdmachinestate.h"
#include "proc/kd/kdf11/cpudata/kdf11cpudata.h"
#include "devicecommon/registerhandler/registerhandler.h"
#include "proc/kd/kdf11/serial_line_units/serial_line_units.h"
#include "bdv11/bdv11.h"
#include "proc/common/composite_cpucontroller/composite_cpucontroller.h"
#include "proc/kd/kdf11/executor/executor.h"
#include "proc/kd/kdf11/calculator/calculator.h"
#include "proc/kd/kdf11/haltmode/haltmode.h"
#include "proc/kd/kdf11/kdf11processorexceptionhandler/kdf11processorexceptionhandler.h"
#include "proc/common/datapaths/datapaths.h"

#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

// The KDF11-BA is a quad-height PDP-11 CPU module (M8189). This module
// contains a central processor, memory management unit (MMU), a line
// frequency clock, a BDV11-compatible bootstrap and diagnostic ROM, and two
// serial-line units. Three extra 40-pin sockets are provided for optional
// floating point and commercial instruction sets. The central processor and
// memory management units are functionally compatible with the KDF11-AA CPU
// and MMU.
//
// The KDF11-BA CPU supports up to 256K bytes of memory on a traditional
// LSI-11 bus backplane (18 address bits) or up to 4 megabytes of memory when
// the module is installed in an extended LSI-11 bus backplane (H9276 or
// H9275). The extended LSI-11 bus backplane adds four address lines to the
// LS11-bus to provide a 22-bit addressing capability when the KDF11-BA is
// used with the MSV11-P (M8067) memory module.
//
// The central processor uses the LSI-11 bus or extended LSI-11 bus with
// 4-level interrupt bus protocol. The KDF11-BA is compatible with existing
// LSI-11 processors and peripheral devices. (EK-KDFEB-UG-001)
//
class KDF11_B : public PDP11Processor
{
public:
    KDF11_B (Bus* bus, const KDF11_BConfig& kdf11_bConfig);
    ~KDF11_B ();
    
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

    // Definition of the KDF11-A components. The KTF11-A (MMU) is an optional
    // component.
    KDF11CpuData cpuData_ {};
    KTF11_A mmu_ {bus_, &cpuData_};
    DataPaths dataPaths_ {bus_, &mmu_};
    CompositeCpuController<KDF11_Executor, KDF11_Calculator,
        KDF11_HaltMode, KDF11ProcessorExceptionHandler> cpuController_ {bus_,
        &cpuData_, &dataPaths_};
    unique_ptr<KDMachineState> machineState_;
    unique_ptr<SerialLineUnits> serialLineUnits;
    unique_ptr<BDV11> bdv11;

    // RegisterHandler performs the functions required by the BusDevice
    // interface. These functions are put in a separate class as they are used
    // by both the KDF11_A and KDF11_B classes.
    unique_ptr<RegisterHandler> registerHandler_;

    // The KDF11_A is started in its own thread
    thread kd11Thread_;

    u16 startAddress_;
};

#endif // !_KDF11_B_H_

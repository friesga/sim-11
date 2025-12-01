#ifndef _KA11_H_
#define _KA11_H_

#include "bus/include/bus.h"
#include "proc/kd/include/pdp11processor.h"
#include "configdata/ka11/ka11config/ka11config.h"
#include "proc/ka11/ky11_a/ky11_a.h"
#include "proc/ka11/ka11machinestate/ka11machinestate.h"
#include "proc/ka11/ka11cpudata/ka11cpudata.h"
#include "panel.h"

#include <memory>

using std::unique_ptr;

// The KA11 is the CPU of the PDP-11/20.
//
class KA11 : public PDP11Processor
{
public:
    KA11 (Bus* bus, Window* window, const KA11Config& ka11Config);

    // Functions required by the BusDevice interface
    CondData<u16> read (BusAddress address);
    StatusCode writeWord (BusAddress address, u16 value);
    StatusCode writeByte (BusAddress address, u8 value);
    bool responsible (BusAddress address);
    void reset ();

    // Functions required by the PDP11Processor interface.
    void start (u16 startAddress);
    void start ();

private:
    Bus* bus_;

    // Definition of the KA11 components.
    KA11CpuData cpuData_ {};
    // ToDo: KDF11_CpuControl cpuControl_ {bus_, &cpuData_, &mmu_};
    unique_ptr<KY11_A> ky11_a_;
    unique_ptr<KA11MachineState> ka11MachineState_;
};

#endif _KA11_H_
#ifndef _KA11MACHINESTATE_H_
#define _KA11MACHINESTATE_H_

#include "proc/common/basemachinestate/basemachinestate.h"

class KA11MachineState : public BaseMachineState
{
public:
    KA11MachineState (Bus* bus, CpuData* cpuData,
        Interfaces::CpuController* cpuControl, MMU* mmu);

    // Functions required by BaseMachineState
    State powerUpRoutine () override;
    State powerDownRoutine () override;
    State bootRoutine () override;
    void runODT () override;
};


#endif // _KA11MACHINESTATE_H_ 
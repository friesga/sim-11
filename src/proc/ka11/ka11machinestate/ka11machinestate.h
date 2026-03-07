#ifndef _KA11MACHINESTATE_H_
#define _KA11MACHINESTATE_H_

#include "proc/common/basemachinestate/basemachinestate.h"
#include "proc/include/ky11console.h"

class KA11MachineState : public BaseMachineState
{
public:
    KA11MachineState (Bus* bus, CpuData* cpuData,
        Interfaces::CpuController* cpuController, MMU* mmu,
        KY11Console& ky11Console);

    // Functions required by BaseMachineState
    State powerUpRoutine () override;
    State powerDownRoutine () override;
    State bootRoutine () override;
    void runODT () override;

private:
    KY11Console& ky11Console_;
};


#endif // _KA11MACHINESTATE_H_ 
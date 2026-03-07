#ifndef _KDMACHINESTATE_H_
#define _KDMACHINESTATE_H_

#include "proc/common/basemachinestate/basemachinestate.h"

class KDMachineState : public BaseMachineState
{
public:
    KDMachineState (Bus* bus, CpuData* cpuData, Interfaces::CpuController* cpuController,
        MMU* mmu, KD11Config::PowerUpMode powerUpMode, u16 startAddress,
        KD11ODT::Creator odtCreator);

    // Functions required by BaseMachineState
    State powerUpRoutine () override;
    State powerDownRoutine () override;
    State bootRoutine () override;
    void runODT () override;

private:
    KD11Config::PowerUpMode powerUpMode_;
    u16 startAddress_;
    KD11ODT::Creator odtCreator_;
    unique_ptr<KD11ODT>	odt_ {};
};


#endif // _KA11MACHINESTATE_H_ 
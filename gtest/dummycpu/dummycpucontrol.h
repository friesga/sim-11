#ifndef _DUMMYCPUCONTROL_H_
#define _DUMMYCPUCONTROL_H_

#include "proc/include/cpucontrol.h"

class DummyCpuControl : public Interfaces::CpuController
{
public:
    // Definitions required for the CpuControl interface
    void cpuReset () {};
    void busReset () {};
    void halt (Interfaces::CpuController::HaltReason reason) {};
    void setHaltMode (bool haltMode) {};
    bool inHaltMode () { return false; };
    void wait () {};
    void start (u16 address) {};
    void proceed () {};
    HaltReason haltReason () { return HaltReason::HaltInstruction; };
    CpuRunState execute () { return CpuRunState::RUN; };
};

#endif // _DUMMYCPUCONTROL_H_
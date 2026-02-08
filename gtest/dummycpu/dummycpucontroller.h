#ifndef _DUMMYCPUCONTROLLER_H_
#define _DUMMYCPUCONTROLLER_H_

#include "proc/include/cpucontroller.h"

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
    bool pushWord (u16 value) { return false; };
    bool popWord (u16* destination) { return false; };
};

#endif // _DUMMYCPUCONTROLLER_H_
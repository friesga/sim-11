#include "kd11cpudata.h"

// Constructor
KD11CpuData::KD11CpuData (Qbus* bus)
    :
    bus_ {bus},
    psw_ {0},
    trap_ {nullptr}
{}

// Generate the given trap using the interrupt request mechanism
void KD11CpuData::setTrap (InterruptRequest const* trap)
{
    trap_ = trap;
}
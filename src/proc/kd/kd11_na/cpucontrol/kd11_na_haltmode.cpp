#include "kd11_na_cpucontrol.h"

// The HaltMode is not implemented in the KD11-NA. inHaltMode() returns
// false to prevent the CPU being halted by an unsupported feature.
// 
void KD11_NA_CpuControl::setHaltMode (bool haltMode)
{}

bool KD11_NA_CpuControl::inHaltMode ()
{
    return false;
}

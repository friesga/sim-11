#include "kd11cpudata.h"
#include "kd/include/psw.h"

// Constructor
KD11CpuData::KD11CpuData (Qbus* bus)
    :
    bus_ {bus},
    psw_ {0},
    trap_ {CpuData::Trap::None}
{}

// Generate the given trap using the interrupt request mechanism
void KD11CpuData::setTrap (CpuData::Trap trap, TrapRecordType cause)
{
    trace.trap (cause, trap);
    trap_ = trap;
}

// Calculate the new condition codes from the current condition codes and
// the condition codes resulting from execution of an instruction and
// assign these to the condition codes in the PSW.
void KD11CpuData::setCC (ConditionCodes conditionCodes)
{
    psw_ = (psw_ & ~(PSW_C | PSW_V | PSW_Z | PSW_N)) | 
        conditionCodes.updatedConditionCodes (psw_);
}
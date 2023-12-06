#include "kdf11_acpudata.h"
#include "kd/include/psw.h"

// Constructor
KDF11_ACpuData::KDF11_ACpuData (Qbus* bus)
    :
    bus_ {bus},
    psw_ {0},
    trap_ {CpuData::TrapCondition::None}
{}

// Generate the given trap using the interrupt request mechanism
void KDF11_ACpuData::setTrap (CpuData::TrapCondition trap, TrapRecordType cause)
{
    trace.trap (cause, trapVector (trap));
    trap_ = trap;
}

// Calculate the new condition codes from the current condition codes and
// the condition codes resulting from execution of an instruction and
// assign these to the condition codes in the PSW.
void KDF11_ACpuData::setCC (ConditionCodes conditionCodes)
{
    psw_ = (psw_ & ~(PSW_C | PSW_V | PSW_Z | PSW_N)) | 
        conditionCodes.updatedConditionCodes (psw_);
}
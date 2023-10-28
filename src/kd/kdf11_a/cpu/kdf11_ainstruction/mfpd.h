#ifndef _MFPD_H_
#define _MFPD_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// MFPD - Move from previous data space
// MFPI - Move from previous instruction space
//
// Operation:
//  (tmp) <- (src)
//  -(SP) <- (tmp)
//
// Condition Codes: 
//  N: set if the source < 0 
//  Z: set if the source = 0 
//  V: cleared 
//  C: unaffected 
//
// Pushes a word onto the current stack from an address in the previous space.
// The source address is computed using the current registers and memory map.
// Since data space does not exist in the KDFll, MFPD executes in the same way
// as an MFPI does. 
//
class KDF11_AInstruction::MFPD : public SingleOperandInstruction, public WithFactory<MFPD>
{
public:
    MFPD (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::MFPD::MFPD (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline bool KDF11_AInstruction::MFPD::execute ()
{
    CondData<u16> source;

    // The source operand is determined in the current memory management
    // mode and then retrieved using the previous mode.
    operandLocation_ =  getOperandLocation (cpu_->registers ());
    source = operandLocation_.prevModeContents<CondData<u16>> ();

    if (!source.hasValue ())
        return false;

    if (!cpu_->pushWord (source))
        return false;

    setPSW (ConditionCodes {.N = (bool) (source & 0100000),
        .Z = source == 0,
        .V = false});

    return true;
}

#endif // _MFPD_H_
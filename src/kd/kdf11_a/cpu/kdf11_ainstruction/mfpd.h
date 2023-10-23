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
    CpuData::Trap execute () override;
};

inline KDF11_AInstruction::MFPD::MFPD (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap KDF11_AInstruction::MFPD::execute ()
{
    CondData<u16> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

    cpu_->pushWord (source);

    setPSW (ConditionCodes {.N = (bool) (source & 0100000),
        .Z = source == 0,
        .V = false});

    return CpuData::Trap::None;
}

#endif // _MFPD_H_
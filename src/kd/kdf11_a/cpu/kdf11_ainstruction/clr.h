#ifndef _CLR_H_
#define _CLR_H_

#include "kd/kdf11_a/cpu/kdf11_ainstruction/kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// CLR - clear destination
//
// Operation:
//  (dst) <- 0
//
// Condition Codes:
//  N: cleared
//  Z: set
//  V: cleared
//  C: cleared
//
// Contents of specified destination are replaced with zeroes.
//
// This class implements the CLR instruction for the KDF11-A. This processor
// allows access to the PSW via address 0177776. As JKDBD0 test 243 shows,
// a CLR of the PSW does not set the condition codes. Therefore the condition
// codes are set before the CLR is executed. As a consequence the original
// conditions codes have to be restored when clearance of the destionation
// operand location fails. The setPSW() can be used for this purpose as the
// T-bit is not affected by an Explicit PS Access (see EK-KDF-UG-PR2,
// Table 8-1).
//
class KDF11_AInstruction::CLR : public SingleOperandInstruction, public WithFactory<CLR>
{
public:
    CLR (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline KDF11_AInstruction::CLR::CLR (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap KDF11_AInstruction::CLR::execute ()
{
    u16 originalPsw = cpu_->pswValue ();

    clearConditionCode (PSW_N | PSW_V | PSW_C);
    setConditionCode (PSW_Z);

    if (!writeOperand ((u16) 0))
    {
        cpu_->setPSW (originalPsw);
        return CpuData::Trap::BusError;
    }

    return CpuData::Trap::None;
}

#endif // _CLR_H_
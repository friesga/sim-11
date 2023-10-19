#ifndef _CLRB_H_
#define _CLRB_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/clrb.h"

// CLRB - clear destination byte
//
// Operation:
//  refer to CLR
// 
// Condition Codes:
//  refer to CLR
//
// This class implements the CLRB instruction for the KDF11-A. See comment
// in KDF11_AInstruction::MOV.
//
class KDF11_AInstruction::CLRB : public SingleOperandInstruction, public WithFactory<CLRB>
{
public:
    CLRB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline KDF11_AInstruction::CLRB::CLRB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap KDF11_AInstruction::CLRB::execute ()
{
    u16 originalPsw = cpu_->pswValue ();

    clearConditionCode (PSW_N | PSW_V | PSW_C);
    setConditionCode (PSW_Z);

    if (!writeOperand ((u8) 0))
    {
        cpu_->setPSW (originalPsw);
        return CpuData::Trap::BusError;
    }

    return CpuData::Trap::None;
}

#endif // _CLRB_H_
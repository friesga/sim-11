#ifndef _MTPD_H_
#define _MTPD_H_

#include "kdf11instruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// MTPD - Move to previous data space
// MTPI - Move to previous instruction space
//
// Operation:
//  (tmp) <- (SP)+
//  (dst) <- (tmp)
//
// Condition Codes: 
//  N: set if the source < 0 
//  Z: set if the source = 0 
//  V: cleared 
//  C: unaffected 
//
// This instruction pops a word off the current stack determined by PS bits
// 15 and 14 and stores that word into an address in the previous space (PS
// bits 13 and 12). The destination address is computed using the current
// registers and memory map. 
//
// Since data space does not exist in the KDF11, MTPD executes in the same way 
// as MTPI does. 
//
class KDF11Instruction::MTPD : public SingleOperandInstruction, public WithFactory<MTPD>
{
public:
    MTPD (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::MTPD::MTPD (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11Instruction::MTPD::execute ()
{
    u16 tmp;
    
    // The destination operand and the value popped off the stack are 
    // retrieved in the current memory management and the tmp value then
    // is written using the previous mode.
    operandLocation_ =  getOperandLocation (cpuData_->registers ());

    if (!mmu_->popWord (&tmp) || !operandLocation_.writePrevMode (tmp))
        return false;
        
    setPSW (ConditionCodes {.N = (bool) (tmp & 0100000),
        .Z = tmp == 0,
        .V = false});

    return true;
}

#endif // _MTPD_H_
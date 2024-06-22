#ifndef _BASEEXECUTOR_H_
#define _BASEEXECUTOR_H_

#include "kd/common/lsi11instruction/lsi11instruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/mmu.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/operand/operand.h"

class BaseExecutor
{
public:
    BaseExecutor (CpuData* cpuData, MMU* mmu);

protected:
    LSI11Instruction* instr_ {nullptr};

    void setPSW (ConditionCodes conditionCodes);
	constexpr bool isSet (u16 x);
    OperandLocation decodeOperand (Operand operand, GeneralRegisters &reg);

private:
    CpuData* cpuData_;
    MMU* mmu_;

    bool isByteInstruction ();
};

constexpr bool BaseExecutor::isSet (u16 x)
{
	return (cpuData_->psw () & x) ? true : false;
}

#endif // _BASEEXECUTOR_H_
#ifndef _BRANCHINSTRUCTION_H_
#define _BRANCHINSTRUCTION_H_

#include "kd/common/lsi11instruction/lsi11instruction.h"
#include "kd/include/cpudata.h"

class BranchInstruction : public LSI11Instruction
{
    union instr
	{
		u16 word;
		struct
		{
			u16	offset:8;
			u16	opcode:8;
		} 
		decoded;
	} 
	instr_;

public:
	BranchInstruction (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
	u16 getOperationCode () override;

protected:
	s8 getOffset ();
	constexpr void executeBranchIf (bool condition);
	void executeBranch ();
};

// Execute the branch given in the instruction under the given condition.
constexpr void BranchInstruction::executeBranchIf (bool condition)
{
    if (condition)
        executeBranch ();
}

#endif // _BRANCHINSTRUCTION_H_
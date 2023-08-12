#ifndef _BRANCHINSTRUCTION_H_
#define _BRANCHINSTRUCTION_H_

#include "../lsi11instruction/lsi11instruction.h"
#include "../cpudata.h"

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
	BranchInstruction (CpuData* cpu, u16 instruction);
	
	s8 getOffset ();
	u16 getOperationCode () override;

protected:
	constexpr void executeBranchIf (bool condition);
	void executeBranch ();
};

#endif // _BRANCHINSTRUCTION_H_
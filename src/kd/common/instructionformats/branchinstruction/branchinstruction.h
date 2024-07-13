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
	BranchInstruction (u16 instruction);
	s8 getOffset () const;
	u16 getOperationCode () const override;
};

#endif // _BRANCHINSTRUCTION_H_
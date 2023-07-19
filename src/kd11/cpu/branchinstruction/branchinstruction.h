#ifndef _BRANCHINSTRUCTION_H_
#define _BRANCHINSTRUCTION_H_

#include "types.h"

class BranchInstruction
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
	s8 getOffset ();
};

#endif // _BRANCHINSTRUCTION_H_
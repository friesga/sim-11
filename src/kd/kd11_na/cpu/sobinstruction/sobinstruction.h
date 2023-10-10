#ifndef _SOBINSTRUCTION_H_
#define _SOBINSTRUCTION_H_

#include "../kd11_na_cpu.h"
#include "kd/common/lsi11instruction/lsi11instruction.h"

class SobInstruction : public LSI11Instruction
{
    // The SOB instruction format comprises the operation code, the register
	// and an offset.
	union instr
	{
		u16 word;
		struct
		{
			u16	offset:6;
			u16	rn:3;
			u16	opcode:7;
		} 
		decoded;
	} 
	instr_;

public:
	SobInstruction (CpuData* cpu, u16 instruction);
	u16 getOperationCode ();

protected:
	u16 getRegisterNr ();
	u16 getOffset ();
};


#endif // _SOBINSTRUCTION_H_
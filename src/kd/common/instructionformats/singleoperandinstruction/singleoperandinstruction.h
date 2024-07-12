#ifndef _SINGLEOPERANDINSTRUCTION_H_
#define _SINGLEOPERANDINSTRUCTION_H_

#include "kd/common/lsi11instruction/lsi11instruction.h"

class SingleOperandInstruction : public LSI11Instruction
{
	// Single operand instruction format comprises the operation code and
	// one operand
	union instr
	{
		u16 word;
		struct
		{
			u16	rn:3;
			u16	mode:3;
			u16	opcode:10;
		} 
		decoded;
	} 
	instr_;

public:
	SingleOperandInstruction (u16 instruction);
	u16 getRegister () const;
	u16 getMode () const ;
	u16 getOperationCode () override;
};

#endif // !_SINGLEOPERANDINSTRUCTION_H_

#ifndef _FISINSTRUCTION_H_
#define _FISINSTRUCTION_H_

#include "kd/common/lsi11instruction/lsi11instruction.h"
#include "float/float.h"

#include <functional>

// The FIS instruction format is used for the FIS instructions, FADD, FSUB,
// FMUL and FDIV. The RTS instruction uses the same format.
class FISInstruction : public LSI11Instruction
{
    union instr
	{
		u16 word;
		struct
		{
			u16	rn:3;
			u16	opcode:13;
		} 
		decoded;
	} 
	instr_;

public:
	FISInstruction (CpuData* cpu, u16 instruction);
	u16 getOperationCode () override;

protected:
	u16 getRegister ();
	CpuData::Trap returnFISresult (Float result, u16 registerNumber);
	CpuData::Trap executeFISinstruction (u16 stackPointer, 
		std::function<bool(Float, Float)> argumentsValid,
		std::function<Float(Float, Float)>);
};

#endif // _FISINSTRUCTION_H_
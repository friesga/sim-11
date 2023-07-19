#ifndef _JSRINSTRUCTION_H_
#define _JSRINSTRUCTION_H_

#include "../kd11cpu.h"
#include "../operandlocation/operandlocation.h"

class JsrInstruction
{
	union instr
	{
		u16 word;
		struct
		{
			u16	registerNr:3;
			u16	mode:3;
			u16	linkageRegister:3;
			u16	opcode:7;
		} 
		decoded;
	} 
	instr_;

	KD11CPU *cpu_;

public:
	JsrInstruction (KD11CPU *cpu, u16 instruction);
	OperandLocation getOperandLocation (u16 (&reg)[8]);
	u16 getLinkageRegister ();
};


#endif // !_JSROPERANDINSTRUCTION_H_

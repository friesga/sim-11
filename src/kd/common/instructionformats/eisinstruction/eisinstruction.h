#ifndef _EISINSTRUCTION_H_
#define _EISINSTRUCTION_H_

#include "kd/common/lsi11instruction/lsi11instruction.h"
#include "kd/common/operandlocation/operandlocation.h"

// The EIS instruction format is used by the EIS instructions MUL, DIV, ASH
// and ASHC instruction and additionally by the JSR and XOR instructions.
class EisInstruction : public LSI11Instruction
{
public:
	EisInstruction (u16 instruction);

	u16 getOperandRegister () const;
	u16 getOperandMode () const;
	u16 getRegisterNr () const;
	u16 getOperationCode () const override;
	int getShiftCount () const;

private:
	union instr
	{
		u16 word;
		struct
		{
			u16	operandRegNr:3;
			u16	operandMode:3;
			u16	registerNr:3;
			u16	opcode:7;
		} 
		decoded;
	} 
	instr_;

	static const u16 sourceOperandMask {077};
	static const u16 signBit {040};
	static const u16 upperBits {0300};

	s8 sixBitTwoComplementToInt (u16 source) const;
};


#endif // !_EISOPERANDINSTRUCTION_H_

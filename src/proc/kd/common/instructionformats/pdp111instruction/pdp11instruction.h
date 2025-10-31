#ifndef _PDP11INSTRUCTION_H_
#define _PDP11INSTRUCTION_H_

#include "types.h"

class PDP11Instruction
{
public:
	// All derived classes have to provide a function returning the operation
	// code of the instruction. This detour to get at the instruction's
	// opcode is necessary as the decoding of the instruction takes places
	// after PDP11Instruction's constructor is called.
	virtual u16 getOperationCode () const = 0;
};

#endif // _PDP11INSTRUCTION_H_
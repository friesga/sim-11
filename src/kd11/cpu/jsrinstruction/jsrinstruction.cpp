#include "jsrinstruction.h"

JsrInstruction::JsrInstruction (KD11CPU *cpu, u16 instruction)
    :
    instr_ {instruction},
    cpu_ {cpu}
{}

// Derive the location of the operand from the addressing mode and the
// register in the instruction and possibly the index and a memory address.
OperandLocation JsrInstruction::getOperandLocation (u16 (&reg)[8])
{
	CondData<u16> addr;
	CondData<u16> index;

	switch (instr_.decoded.mode)
	{
		case 0:
			// Register mode. The operand is contained in the instruction
			// specified register.
			return OperandLocation (cpu_, instr_.decoded.registerNr);

		case 1:
			// Register deferred (indirect) mode. The register contains
			// the address of the operand.
			return OperandLocation (cpu_, 
				CondData<u16> (reg[instr_.decoded.registerNr]));

		case 2:
			// Auto-increment mode. Register is used as a pointer to
			// sequential data then incremented.
			// After incrementing, the address in the register is made even,
			// ToDo: is that correct?
			addr = reg[instr_.decoded.registerNr];
			reg[instr_.decoded.registerNr] += 2;
			reg[instr_.decoded.registerNr] &= 0xFFFE;
			return OperandLocation (cpu_, CondData<u16> (addr));

		case 3: 
			// Auto-increment deferred (indirect) mode. Register is first used
			// as apointer to a word containing the address of the operand, then
			// incremented (always by 2; even for byte instructions).
			addr = reg[instr_.decoded.registerNr];
			reg[instr_.decoded.registerNr] += 2;
			reg[instr_.decoded.registerNr] &= 0xFFFE;
			return OperandLocation (cpu_, CondData<u16> (cpu_->fetchWord (addr)));

		case 4:
			// Auto-decrement mode. Register is decremented and then used as a
			// pointer
			reg[instr_.decoded.registerNr] -= 2;
			addr = reg[instr_.decoded.registerNr];
			reg[instr_.decoded.registerNr] &= 0xFFFE;
			return OperandLocation (cpu_, CondData<u16> (addr));

		case 5:
			// Auto-decrement deferred (indirect) mode. Register is
			// decremented (always by two; even for byte instructions) and
			// then used as a pointer to a word containing the address of the
			// operand.
			reg[instr_.decoded.registerNr] -= 2;
			addr = reg[instr_.decoded.registerNr];
			reg[instr_.decoded.registerNr] &= 0xFFFE;
			return OperandLocation (cpu_, CondData<u16> (cpu_->fetchWord (addr)));

		case 6:
			// Index mode. The contents of the in the instruction specified
			// register and an index word following the instruction word, are
			// summed up to form the address of the operand.
			// The PC already has been incremented so it points to the index
			// word. After fetching the index it has to be incremented again
			// to point to the next instruction. This can be regarded as an
			// undesirable side effect of this function.
			index = cpu_->fetchWord (reg[7]);
			reg[7] += 2;
			return OperandLocation (cpu_, 
				CondData<u16> (reg[instr_.decoded.registerNr] + index));
			
		case 7: 
			// Index deferred (indirect) mode. The value stored in the word
			// following the instruction and the contents of the address in 
			// the specified register are added and the sum is used as a pointer
			// to a word containing the address of the operand.
			// See the comment at mode 6.
			index = cpu_->fetchWord (reg[7]);
			reg[7] += 2;
			addr = reg[instr_.decoded.registerNr] + index;
			return OperandLocation (cpu_, CondData<u16> (cpu_->fetchWord (addr)));

		default:
			// Satisfy the compiler. This cannot happen as the mode bit field
			// is three bits wide.
			throw "Cannot happen";
	}
}

u16 JsrInstruction::getLinkageRegister ()
{
	return instr_.decoded.linkageRegister;
}
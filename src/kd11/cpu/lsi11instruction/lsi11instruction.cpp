#include "lsi11instruction.h"

#include <typeinfo>

LSI11Instruction::LSI11Instruction (CpuData *cpu)
	:
	cpu_ {cpu}
{}

// Get the location of the operand by decoding the adressing mode of the
// operand.
//
// The autoincrement and autodecrement modes (modes 2 and 4) provide for
// automatic stepping of a pointer through sequential elements of a table
// of operands. It assumes the contents of the selected general register to
// be the address of the operand. Contents of registers are stepped (by one
// for bytes, by two for words, always by two for R6 and R7) to address the
// next sequential location.
//
OperandLocation LSI11Instruction::decodeOperand (Operand operand, u16 (&reg)[8])
{
	CondData<u16> addr;
	CondData<u16> index;

	switch (operand.mode_)
	{
		case 0:
			// Register mode. The operand is contained in the instruction
			// specified register.
			return OperandLocation (RegisterOperandLocation {cpu_, (u8) operand.registerNr_});

		case 1:
			// Register deferred (indirect) mode. The register contains
			// the address of the operand.
			return OperandLocation (MemoryOperandLocation {cpu_, 
				CondData<u16> (reg[operand.registerNr_])});

		case 2:
			// Auto-increment mode. Register is used as a pointer to
			// sequential data then incremented.
			addr = reg[operand.registerNr_];
			if (isByteInstruction () && operand.registerNr_ != 6 &&
					operand.registerNr_ != 7)
				++reg[operand.registerNr_];
			else
				reg[operand.registerNr_] += 2;

			return OperandLocation (MemoryOperandLocation {cpu_, 
				CondData<u16> (addr)});

		case 3: 
			// Auto-increment deferred (indirect) mode. Register is first used
			// as apointer to a word containing the address of the operand, then
			// incremented (always by 2; even for byte instructions).
			addr = reg[operand.registerNr_];
			reg[operand.registerNr_] += 2;
			return OperandLocation (MemoryOperandLocation {cpu_, 
				CondData<u16> (cpu_->fetchWord (addr))});

		case 4:
			// Auto-decrement mode. Register is decremented and then used as a
			// pointer
			if (isByteInstruction () && operand.registerNr_ != 6 &&
					operand.registerNr_ != 7)
				--reg[operand.registerNr_];
			else
				reg[operand.registerNr_] -= 2;
			addr = reg[operand.registerNr_];
			return OperandLocation (MemoryOperandLocation {cpu_, 
				CondData<u16> (addr)});

		case 5:
			// Auto-decrement deferred (indirect) mode. Register is
			// decremented (always by two; even for byte instructions) and
			// then used as a pointer to a word containing the address of the
			// operand.
			reg[operand.registerNr_] -= 2;
			addr = reg[operand.registerNr_];
			return OperandLocation (MemoryOperandLocation {cpu_, 
				CondData<u16> (cpu_->fetchWord (addr))});

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
			return OperandLocation (MemoryOperandLocation {cpu_, 
				CondData<u16> (reg[operand.registerNr_] + index)});
			
		case 7: 
			// Index deferred (indirect) mode. The value stored in the word
			// following the instruction and the contents of the address in 
			// the specified register are added and the sum is used as a pointer
			// to a word containing the address of the operand.
			// See the comment at mode 6.
			index = cpu_->fetchWord (reg[7]);
			reg[7] += 2;
			addr = reg[operand.registerNr_] + index;
			return OperandLocation (MemoryOperandLocation {cpu_, 
				CondData<u16> (cpu_->fetchWord (addr))});

		default:
			// Satisfy the compiler. This cannot happen as the mode bit field
			// is three bits wide.
			throw "Cannot happen";
	}
}

// Determine if the given opCode is for a byte instruction. Two ranges of byte
// instructions are defined 01050DD (MOVB) - 01067DD (MFPS) and 
// 011SSDD (MOVB) - 015SSDD (BISB).
//
// The instruction's operation code is retrieved from the derived instruction
// class.
//
bool LSI11Instruction::isByteInstruction ()
{
	u16 opCode = getOperationCode ();
	return (opCode >= 01050 && opCode <= 01067) || 
		   (opCode >= 011 && opCode <= 015) ? true : false;
}

// Return the mnemonic for this instruction from the object's class name.
// The call "typeid (*this).name ()" will return a string with the complete
// type, e.g. "class KD11_NA::HALT". The position of the mnemonic string
// is two characters after the first colon in the type name.
string LSI11Instruction::mnemonic ()
{
	string typeName = typeid (*this).name ();
	return typeName.substr (typeName.find (":") + 2);
}
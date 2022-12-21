#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

#include "kd11cpu.h"
#include "bitmask.h"

// Options for the get and put operand functions, 
// used in Bitmask<OperandOptions>
enum class OperandOptions
{
    Default,                    // Open existing file read/write
    Word,						// Get word operand
    Byte,				        // Get byte operand
    AutoIncr,                   // Increment/decrement oeration
    _                           // Required for Bitmask
};

// The functions in this class get and put the operand, given the
// instructions mode and register.
class Instruction
{
protected:
	bool getWordOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 mode, u16 regNr, u16 &retValue);
	bool getByteOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 mode, u16 regNr, u16 &retValue);
	bool getAddress (KD11CPU *cpu, u16 (&reg)[8], u16 mode, 
		u16 regNr, u16 &retValue);
	bool putWordOperand (KD11CPU *cpu, u16 (&reg)[8], u16 mode, 
		u16 regNr, u16 value);
	bool putByteOperand (KD11CPU *cpu, u16 (&reg)[8], u16 mode,
		u16 regNr, u8 value);
};

bool Instruction::getWordOperand (KD11CPU *cpu, u16 (&reg)[8], 
	Bitmask<OperandOptions> options, u16 mode, u16 regNr, u16 &retValue)
{
	CondData<u16> addr;
	CondData<u16> tmp;

	switch (mode)
	{
		case 0: /* Register */
			retValue = reg[regNr];
			return true;

		case 1: /* Register indirect */
			addr = cpu->fetchWord (reg[regNr]);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		case 2: /* Autoincrement */
			addr = reg[regNr];
			if (options & OperandOptions::AutoIncr) 
			{
				reg[regNr] += 2;
				reg[regNr] &= 0xFFFE;
			}
			tmp = cpu->fetchWord (addr);
			if (!tmp.hasValue()) 
				return false;
			retValue = tmp;
			return true;

		case 3: /* Autoincrement indirect */
			addr = reg[regNr];
			if (options & OperandOptions::AutoIncr) 
			{
				reg[regNr] += 2;
				reg[regNr] &= 0xFFFE;
			}
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue()) 
				return false;

			addr = cpu->fetchWord (addr);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		case 4: /* Autodecrement */
			if (options & OperandOptions::AutoIncr) 
			{
				reg[regNr] -= 2;
				addr = reg[regNr];
				reg[regNr] &= 0xFFFE;
			} 
			else 
				addr = reg[regNr] - 2;

			addr = cpu->fetchWord (addr);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		case 5: /* Autodecrement indirect */
			if (options & OperandOptions::AutoIncr) 
			{
				reg[regNr] -= 2;
				addr = reg[regNr];
				reg[regNr] &= 0xFFFE;
			} 
			else
				addr = reg[regNr] - 2;
			
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue()) 
				return false;
			tmp = cpu->fetchWord (addr);
			if (!tmp.hasValue()) 
				return false;
			retValue = tmp;
			return true;

		case 6: /* Index */
			addr = cpu->fetchWord (reg[7]);
            if (!addr.hasValue()) 
				return false;

			if (options & OperandOptions::AutoIncr) 
				reg[7] += 2;
			else if (regNr == 7) 
				addr += 2;
			
			addr += reg[regNr];
			addr = cpu->fetchWord (addr);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		case 7: /* Index indirect */
			addr = cpu->fetchWord (reg[7]);
            if (!addr.hasValue()) 
				return false;
			if (options & OperandOptions::AutoIncr) 
				reg[7] += 2;
			else if (regNr == 7)
				addr += 2;

			addr += reg[regNr];
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue()) 
				return false;

			addr = cpu->fetchWord (addr);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		default:
			return false;
	}
}

#define	READ8(addr) (((addr) & 1) ? \
    static_cast<CondData<u8>> (cpu->fetchWord((addr) & 0xFFFE) >> 8) : \
	static_cast<CondData<u8>> (cpu->fetchWord(addr & 0xFFFE)))

bool Instruction::getByteOperand (KD11CPU *cpu, u16 (&reg)[8], 
	Bitmask<OperandOptions> options, u16 mode, u16 regNr, u16 &retValue)
{
	CondData<u16> addr;
	CondData<u8> tmp;
	switch (mode)
	{
		case 0: /* Register */
			tmp = (u8) reg[regNr];
			if (!tmp.hasValue())
				return false;
			retValue = tmp;
			return true;

		case 1: /* Register indirect */
			tmp = READ8(reg[regNr]);
			if (!tmp.hasValue())
				return false;
			retValue = tmp;
			return true;

		case 2: /* Autoincrement */
			addr = reg[regNr];
			if (options & OperandOptions::AutoIncr)  
			{
				if(regNr == 6 || regNr == 7) {
					reg[regNr] += 2;
				} else {
					reg[regNr]++;
				}
			}
			tmp = READ8(addr);
			if (!tmp.hasValue())
				return false;
			retValue = tmp;
			return true;

		case 3: /* Autoincrement indirect */
			addr = reg[regNr];
			if (options & OperandOptions::AutoIncr)  
			{
				reg[regNr] += 2;
			}
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue()) 
				return false;
			tmp = READ8(addr);
			if (!tmp.hasValue())
				return false;
			retValue = tmp;
			return true;

		case 4: /* Autodecrement */
			if (options & OperandOptions::AutoIncr)  
			{
				if(regNr == 6 || regNr == 7) {
					reg[regNr] -= 2;
				} else {
					reg[regNr]--;
				}
				addr = reg[regNr];
			} else {
				if(regNr == 6 || regNr == 7) {
					addr = reg[regNr] - 2;
				} else {
					addr = reg[regNr] - 1;
				}
			}
			tmp = READ8(addr);
			if (!tmp.hasValue())
				return false;
			retValue = tmp;

			return true;

		case 5: /* Autodecrement indirect */
			if (options & OperandOptions::AutoIncr)  
			{
				reg[regNr] -= 2;
				addr = reg[regNr];
			} else {
				addr = reg[regNr] - 2;
			}
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue()) 
				return false;
			tmp = READ8(addr);
			if (!tmp.hasValue())
				return false;
			retValue = tmp;
			return true;

		case 6: /* Index */
			addr = cpu->fetchWord (reg[7]);
            if (!addr.hasValue()) 
				return false;
			if (options & OperandOptions::AutoIncr)  
			{
				reg[7] += 2;
			} else if(regNr == 7) {
				addr += 2;
			}
			addr += reg[regNr];
			tmp = READ8(addr);
			if (!tmp.hasValue())
				return false;
			retValue = tmp;
			return true;

		case 7: /* Index indirect */
			addr = cpu->fetchWord (reg[7]);
            if (!addr.hasValue()) 
				return false;
			if (options & OperandOptions::AutoIncr)  
			{
				reg[7] += 2;
			} else if(regNr == 7) {
				addr += 2;
			}
			addr += reg[regNr];
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue()) 
				return false;
			tmp = READ8(addr);
			if (!tmp.hasValue())
				return false;
			retValue = tmp;
			return true;

		default:
			return false;
	}
}

// ToDo: The result of all read() calls should be checked (instead of
// returning zero).
bool Instruction::getAddress (KD11CPU *cpu, u16 (&reg)[8], 
	u16 mode, u16 regNr, u16 &retValue)
{
	CondData<u16> addr;
	CondData<u16> tmp;

	switch (mode)
    {
		case 0: /* Register */
			// This should generate an illegal instruction trap. This
			// has to be performed by the caller.
			// TRCTrap(4, TRC_TRAP_RADDR);
			// TRAP(busError); /* illegal instruction */
            return false;

		case 1: /* Register indirect */
			retValue = reg[regNr];
			return true;

		case 2: /* Autoincrement */
			addr = reg[regNr];
			reg[regNr] += 2;
			retValue = addr;
			return true;

		case 3: /* Autoincrement indirect */
			addr = reg[regNr];
			reg[regNr] += 2;
			addr = cpu->fetchWord (addr);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		case 4: /* Autodecrement */
			reg[regNr] -= 2;
			addr = reg[regNr];
			retValue = addr;
			return true;

		case 5: /* Autodecrement indirect */
			reg[regNr] -= 2;
			addr = reg[regNr];
			addr = cpu->fetchWord (addr);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		case 6: /* Index */
			addr = cpu->fetchWord (reg[7]);
			reg[7] += 2;
			addr += reg[regNr];
			retValue = addr;
			return true;

		case 7: /* Index indirect */
			addr = cpu->fetchWord (reg[7]);
			reg[7] += 2;
			addr += reg[regNr];
			addr = cpu->fetchWord (addr);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		default:
            return false;
	}
}

bool Instruction::putWordOperand (KD11CPU *cpu, u16 (&reg)[8], u16 mode, 
	u16 regNr, u16 value)
{
	CondData<u16> addr;
	switch (mode)
    {
		case 0: /* Register */
			reg[regNr] = value;
            return true;

		case 1: /* Register indirect */
			return cpu->putWord (reg[regNr], value);

		case 2: /* Autoincrement */
			reg[regNr] &= 0xFFFE;
			addr = reg[regNr];
			reg[regNr] += 2;
			return cpu->putWord (addr, value);

		case 3: /* Autoincrement indirect */
			reg[regNr] &= 0xFFFE;
			addr = reg[regNr];
			reg[regNr] += 2;
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue())
                return false;

			return cpu->putWord (addr, value);

		case 4: /* Autodecrement */
			reg[regNr] &= 0xFFFE;
			reg[regNr] -= 2;
			addr = reg[regNr];
			return cpu->putWord (addr, value);

		case 5: /* Autodecrement indirect */
			reg[regNr] &= 0xFFFE;
			reg[regNr] -= 2;
			addr = reg[regNr];
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue())
                return false;
			return cpu->putWord (addr, value);

		case 6: /* Index */
			addr = cpu->fetchWord (reg[7]);
            if (!addr.hasValue())
                return false;
			reg[7] += 2;
			addr += reg[regNr];
			return cpu->putWord (addr, value);

		case 7: /* Index indirect */
			addr = cpu->fetchWord (reg[7]);
            if (!addr.hasValue())
                return false;
			reg[7] += 2;
			addr += reg[regNr];
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue())
                return false;
			return cpu->putWord (addr, value);

        default:
            // Prevent compiler warning on not all paths returning a value
            throw (std::string("Unexpected mode in writeW() call"));
	}
}

bool Instruction::putByteOperand (KD11CPU *cpu, u16 (&reg)[8], u16 mode, u16 regNr, u8 val)
{
	CondData<u16> addr;
	switch(mode)
    {
		case 0: /* Register */
			reg[regNr] = (reg[regNr] & 0xFF00) | val;
			return true;

		case 1: /* Register deferred */
			return cpu->putByte (reg[regNr], val);

		case 2: /* Autoincrement */
			addr = reg[regNr];
			if(regNr == 6 || regNr == 7) {
				reg[regNr] += 2;
			} else {
				reg[regNr]++;
			}
			return cpu->putByte (addr, val);

		case 3: /* Autoincrement deferred */
			addr = reg[regNr];
			reg[regNr] += 2;
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue())
                return false;
			return cpu->putByte (addr, val);

		case 4: /* Autodecrement */
			if(regNr == 6 || regNr == 7) {
				reg[regNr] -= 2;
			} else {
				reg[regNr]--;
			}
			addr = reg[regNr];
			return cpu->putByte (addr, val);

		case 5: /* Autodecrement deferred */
			reg[regNr] -= 2;
			addr = reg[regNr];
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue())
                return false;
			return cpu->putByte (addr, val);

		case 6: /* Index */
			addr = cpu->fetchWord (reg[7]);
            if (!addr.hasValue())
                return false;
			reg[7] += 2;
			addr += reg[regNr];
			return cpu->putByte (addr, val);

		case 7: /* Index deferred */
			addr = cpu->fetchWord (reg[7]);
            if (!addr.hasValue())
                return false;
			reg[7] += 2;
			addr += reg[regNr];
			addr = cpu->fetchWord (addr);
            if (!addr.hasValue())
                return false;
			return cpu->putByte (addr, val);

        default:
            // Prevent compiler warning on not all paths returning a value
            throw (std::string("Unexpected mode in writeB() call"));
	}
}


// (Try to) determine the byte order. To that end gcc provides the __BYTE__ORDER__
// definition. Msvc has no such definition but we can safely assume that all
// win32 machines are little endian.
#if _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
// Little endian host

// Instruction format for single operand instructions
// 
// For auto-increment and auto-decrement instructions, the increment or
// decrement is performed in the last function retrieving the operand.
// Therefore there are two versions of the getOperand() instructions,
// one without auto-inc/decrement and one performing the increment/
// decrement.
struct KD11INSN1 : public Instruction
{
	u16	rn:3;
	u16	mode:3;
	u16	opcode:10;

	bool getOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 &retValue);
	bool getAddress (KD11CPU *cpu, u16 (&reg)[8], u16 &retValue);
	bool putOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 val);
};

bool KD11INSN1::getOperand (KD11CPU *cpu, u16 (&reg)[8], 
	Bitmask<OperandOptions> options, u16 &retValue)
{
	if (options & OperandOptions::Byte)
		return Instruction::getByteOperand (cpu, reg, options, mode, rn, retValue);
	else if (options & OperandOptions::Word)
		return Instruction::getWordOperand (cpu, reg, options, mode, rn, retValue);
	else
		throw (std::string("Missing getOperand option"));
}

bool KD11INSN1::getAddress (KD11CPU *cpu, u16 (&reg)[8], u16 &retValue)
{
	return Instruction::getAddress (cpu, reg, mode, rn, retValue);
}

bool KD11INSN1::putOperand (KD11CPU *cpu, u16 (&reg)[8], 
	Bitmask<OperandOptions> options, u16 value)
{
	if (options & OperandOptions::Byte)
		return Instruction::putByteOperand (cpu, reg, mode, rn, value);
	else if (options & OperandOptions::Word)
		return Instruction::putWordOperand (cpu, reg, mode, rn, value);
	else
		throw (std::string("Missing putOperand option"));
}

struct KD11INSN2 : public Instruction
{
	u16	dst_rn:3;
	u16	dst_mode:3;
	u16	src_rn:3;
	u16	src_mode:3;
	u16	opcode:4;

	bool getSourceOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 &retValue);
	bool getDestOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 &retValue);
	bool putDestOperand (KD11CPU *cpu, u16 (&reg)[8],
		Bitmask<OperandOptions> options, u16 val);
};

bool KD11INSN2::getSourceOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 &retValue)
{
	if (options & OperandOptions::Byte)
		return Instruction::getByteOperand (cpu, reg, options,
			src_mode, src_rn, retValue);
	else if (options & OperandOptions::Word)
		return Instruction::getWordOperand (cpu, reg, options, 
			src_mode, src_rn, retValue);
	else
		throw (std::string("Missing getSourceOperand option"));
}

bool KD11INSN2::getDestOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 &retValue)
{
	if (options & OperandOptions::Byte)
		return Instruction::getByteOperand (cpu, reg, options,
			dst_mode, dst_rn, retValue);
	else if (options & OperandOptions::Word)
		return Instruction::getWordOperand (cpu, reg, options, 
			dst_mode, dst_rn, retValue);
	else
		throw (std::string("Missing getDestOperand option"));
}

bool KD11INSN2::putDestOperand (KD11CPU *cpu, u16 (&reg)[8], 
	Bitmask<OperandOptions> options, u16 value)
{
	if (options & OperandOptions::Byte)
		return Instruction::putByteOperand (cpu, reg, dst_mode, dst_rn, value);
	else if (options & OperandOptions::Word)
		return Instruction::putWordOperand (cpu, reg, dst_mode, dst_rn, value);
	else
		throw (std::string("Missing putDestOperand option"));
}

struct KD11INSNBR
{
	u16	offset:8;
	u16	opcode:8;
};

struct KD11INSNJSR : public Instruction
{
	u16	rn:3;
	u16	mode:3;
	u16	r:3;
	u16	opcode:7;

	bool getOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 &retValue);
	bool getAddress (KD11CPU *cpu, u16 (&reg)[8], u16 &retValue);
	bool putOperand (KD11CPU *cpu, u16 (&reg)[8], u16 val);
};

bool KD11INSNJSR::getOperand (KD11CPU *cpu, u16 (&reg)[8], 
	Bitmask<OperandOptions> options, u16 &retValue)
{
	if (options & OperandOptions::Byte)
		return Instruction::getByteOperand (cpu, reg, options, mode, rn, retValue);
	else if (options & OperandOptions::Word)
		return Instruction::getWordOperand (cpu, reg, options, mode, rn, retValue);
	else
		throw (std::string("Missing getOperand option"));
}

bool KD11INSNJSR::getAddress (KD11CPU *cpu, u16 (&reg)[8], u16 &retValue)
{
	return Instruction::getAddress (cpu, reg, mode, rn, retValue);
}

bool KD11INSNJSR::putOperand (KD11CPU *cpu, u16 (&reg)[8], u16 value)
{
	return Instruction::putWordOperand (cpu, reg, mode, rn, value);
}

struct KD11INSNRTS
{
	u16	rn:3;
	u16	opcode:13;
};

struct KD11INSNMARK
{
	u16	nn:6;
	u16	opcode:10;
};

struct KD11INSNSOB
{
	u16	offset:6;
	u16	rn:3;
	u16	opcode:7;
};
#else
/* big endian host */
// ToDo: Add operand instructions to the KD11INS structs
struct KD11INSN1
{
	u16	opcode:10;
	u16	mode:3;
	u16	rn:3;
};

struct KD11INSN2
{
	u16	opcode:4;
	u16	src_mode:3;
	u16	src_rn:3;
	u16	dst_mode:3;
	u16	dst_rn:3;
};

struct KD11INSNBR
{
	u16	opcode:8;
	u16	offset:8;
};

struct KD11INSNJSR
{
	u16	opcode:7;
	u16	r:3;
	u16	mode:3;
	u16	rn:3;
};

struct KD11INSNRTS
{
	u16	opcode:13;
	u16	rn:3;
};

struct KD11INSNMARK
{
	u16	opcode:10;
	u16	nn:6;
};

struct KD11INSNSOB
{
	u16	opcode:7;
	u16	rn:3;
	u16	offset:6;
};
#endif // _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#endif // _INSTRUCTIONS_H_
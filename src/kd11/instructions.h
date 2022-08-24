#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

class Instruction
{
public:
	bool getWordOperand (QBUS* bus, u16 (&reg)[8], bool inc, 
		u16 mode, u16 regNr, u16 &retValue);
	bool getByteOperand (QBUS* bus, u16 (&reg)[8], bool inc, 
		u16 mode, u16 regNr, u16 &retValue);
};

// getWordOperand is the most general version of the functions to get an
// operand from an instruction. More specific versions use this generic
// version to get their operand(s).
bool Instruction::getWordOperand (QBUS* bus, u16 (&reg)[8], bool inc, 
		u16 mode, u16 regNr, u16 &retValue)
{
	CondData<u16> addr;
	switch (mode)
	{
		case 0: /* Register */
			retValue = reg[regNr];
			return true;

		case 1: /* Register indirect */
			addr = bus->read(reg[regNr]);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		case 2: /* Autoincrement */
			addr = reg[regNr];
			if (inc) 
			{
				reg[regNr] += 2;
				reg[regNr] &= 0xFFFE;
			}
			retValue = bus->read (addr);
			return true;

		case 3: /* Autoincrement indirect */
			addr = reg[regNr];
			if (inc) 
			{
				reg[regNr] += 2;
				reg[regNr] &= 0xFFFE;
			}
			addr = bus->read (addr);
            if (!addr.hasValue()) 
				return false;

			addr = bus->read (addr);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		case 4: /* Autodecrement */
			if (inc)
			{
				reg[regNr] -= 2;
				addr = reg[regNr];
				reg[regNr] &= 0xFFFE;
			} 
			else 
				addr = reg[regNr] - 2;

			addr = bus->read (addr);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		case 5: /* Autodecrement indirect */
			if (inc)
			{
				reg[regNr] -= 2;
				addr = reg[regNr];
				reg[regNr] &= 0xFFFE;
			} 
			else
				addr = reg[regNr] - 2;
			
			addr = bus->read (addr);
            if (!addr.hasValue()) 
				return false;
			retValue = bus->read (addr);
			return true;

		case 6: /* Index */
			addr = bus->read (reg[7]);
            if (!addr.hasValue()) 
				return false;

			if (inc)
				reg[7] += 2;
			else if (regNr == 7) 
				addr += 2;
			
			addr += reg[regNr];

			addr = bus->read (addr);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		case 7: /* Index indirect */
			addr = bus->read (reg[7]);
            if (!addr.hasValue()) 
				return false;
			if (inc)
				reg[7] += 2;
			else if (regNr == 7)
				addr += 2;

			addr += reg[regNr];
			addr = bus->read (addr);
            if (!addr.hasValue()) 
				return false;

			addr = bus->read (addr);
			if (!addr.hasValue()) 
				return false;
			retValue = addr;
			return true;

		default:
			return false;
	}
}

#define	READ8(addr) (((addr) & 1) ? \
    static_cast<CondData<u8>> (bus->read((addr) & 0xFFFE) >> 8) : \
	static_cast<CondData<u8>> (bus->read(addr & 0xFFFE)))

bool Instruction::getByteOperand (QBUS* bus, u16 (&reg)[8], bool inc, 
		u16 mode, u16 regNr, u16 &retValue)
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
			if (inc) {
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
			if (inc) {
				reg[regNr] += 2;
			}
			addr = bus->read (addr);
            if (!addr.hasValue()) 
				return false;
			tmp = READ8(addr);
			if (!tmp.hasValue())
				return false;
			retValue = tmp;
			return true;

		case 4: /* Autodecrement */
			if (inc) {
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
			retValue = READ8(addr);
			return true;

		case 5: /* Autodecrement indirect */
			if(inc) {
				reg[regNr] -= 2;
				addr = reg[regNr];
			} else {
				addr = reg[regNr] - 2;
			}
			addr = bus->read (addr);
            if (!addr.hasValue()) 
				return false;
			retValue = READ8(addr);
			return true;

		case 6: /* Index */
			addr = bus->read (reg[7]);
            if (!addr.hasValue()) 
				return false;
			if (inc) {
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
			addr = bus->read (reg[7]);
            if (!addr.hasValue()) 
				return false;
			if (inc) {
				reg[7] += 2;
			} else if(regNr == 7) {
				addr += 2;
			}
			addr += reg[regNr];
			addr = bus->read (addr);
            if (!addr.hasValue()) 
				return false;
			retValue = READ8(addr);
			return true;

		default:
			return false;
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

	bool getWordOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
	bool getWordAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
	bool getByteOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
	bool getByteAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
};

bool KD11INSN1::getWordOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return Instruction::getWordOperand (bus, reg, false, mode, rn, retValue);
}

bool KD11INSN1::getWordAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return Instruction::getWordOperand (bus, reg, true, mode, rn, retValue);
}

bool KD11INSN1::getByteOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return Instruction::getByteOperand (bus, reg, false, mode, rn, retValue);
}

bool KD11INSN1::getByteAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return Instruction::getByteOperand (bus, reg, true, mode, rn, retValue);
}


struct KD11INSN2 : public Instruction
{
	u16	dst_rn:3;
	u16	dst_mode:3;
	u16	src_rn:3;
	u16	src_mode:3;
	u16	opcode:4;

	bool getSourceOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
	bool getSourceAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
	bool getSourceByteOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
	bool getSourceByteAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
	bool getDestOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
	bool getDestAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
	bool getDestByteOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
	bool getDestByteAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue);
};

bool KD11INSN2::getSourceOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return getWordOperand (bus, reg, false, src_mode, src_rn, retValue);
}

bool KD11INSN2::getSourceAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return getWordOperand (bus, reg, true, src_mode, src_rn, retValue);
}

bool KD11INSN2::getSourceByteOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return getByteOperand (bus, reg, false, src_mode, src_rn, retValue);
}

bool KD11INSN2::getSourceByteAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return getByteOperand (bus, reg, true, src_mode, src_rn, retValue);
}

bool KD11INSN2::getDestOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return getWordOperand (bus, reg, false, dst_mode, dst_rn, retValue);
}

bool KD11INSN2::getDestAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return getWordOperand (bus, reg, true, dst_mode, dst_rn, retValue);
}

bool KD11INSN2::getDestByteOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return getByteOperand (bus, reg, false, dst_mode, dst_rn, retValue);
}

bool KD11INSN2::getDestByteAutoIncOperand (QBUS* bus, u16 (&reg)[8], u16 &retValue)
{
	return getByteOperand (bus, reg, true, dst_mode, dst_rn, retValue);
}

struct KD11INSNBR
{
	u16	offset:8;
	u16	opcode:8;
};

struct KD11INSNJSR
{
	u16	rn:3;
	u16	mode:3;
	u16	r:3;
	u16	opcode:7;
};

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
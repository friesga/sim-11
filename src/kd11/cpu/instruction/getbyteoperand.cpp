#include "instruction.h"

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

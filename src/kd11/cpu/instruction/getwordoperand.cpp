#include "instruction.h"

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

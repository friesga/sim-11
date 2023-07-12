#include "instruction.h"

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
			// trap(4, TRC_TRAP_RADDR);
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

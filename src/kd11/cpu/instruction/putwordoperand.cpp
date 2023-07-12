#include "instruction.h"

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

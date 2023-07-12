#include "instruction.h"

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

#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "../kd11cpu.h"
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

#endif // _INSTRUCTION_H_
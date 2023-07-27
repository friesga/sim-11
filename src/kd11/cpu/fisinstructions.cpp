#include "kd11cpu.h"
#include "fisinstruction/fisinstruction.h"

// Execute a FADD, FSUB, FMUL or FDIV instruction.
void KD11CPU::executeFISinstruction (u16 stackPointer,
    std::function<bool(Float, Float)> argumentsValid,
    std::function<Float(Float, Float)> instruction)
{
    // Clear PSW bits 5 and 6
    psw &= ~(_BV(5) | _BV(6));

    CondData<u16> f1High = fetchWord (register_[stackPointer] + 4);
    CondData<u16> f1Low = fetchWord (register_[stackPointer] + 6);
    CondData<u16> f2High = fetchWord (register_[stackPointer]);
    CondData<u16> f2Low = fetchWord (register_[stackPointer] + 2);

    if (f1High.hasValue () && f1Low.hasValue () &&
        f2High.hasValue () && f2Low.hasValue ())
    {
        Float f1 (f1High, f1Low);
        Float f2 (f2High, f2Low);

        if (argumentsValid (f1, f2))
        {
            Float f3 = instruction (f1, f2);
            returnFISresult (f3, stackPointer);
        }
        else
        {
            // The arguments are invalid. This is notably a division
            // by zero
            PSW_SET (PSW_N);
            PSW_CLR (PSW_Z);
            PSW_SET (PSW_V);
            PSW_SET (PSW_C);
            setTrap (&FIS);
        }
    }
}

// FADD - floating add
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6]+[(R),(R)+2],
//  if result >= 2^-128; else [(R)+4, (R)+6] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: cleared
//
// Adds the A argument to the B argument and stores the result in the
// A Argument position on the stack. General register R is used as the stack
// pointer for the operation.
// 
// A <- A + B
//
void KD11CPU::FADD (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    FISInstruction fisInstruction (cpu, instruction);
    executeFISinstruction (fisInstruction.getRegister (),
        [] (Float f1, Float f2) {return true;},
        [] (Float f1, Float f2) {return f1 + f2;});
}

// FSUB - floating subtract
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6] - [(R), (R)+2],
//  if result >= 2^-128; else [(R)+4, (R)+6] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: cleared
//  C: cleared
//
// Sutracts the B Argument from the A Argument and stores the result in
// the A Argument position on the stack.
//
// A <- A - B
//
void KD11CPU::FSUB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    FISInstruction fisInstruction (cpu, instruction);
    executeFISinstruction (fisInstruction.getRegister (),
        [] (Float f1, Float f2) {return true;},
        [] (Float f1, Float f2) {return f1 - f2;});
}

// FMUL - floating multiply
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6] * [(R), (R)+2]
//  if result >= 2^-128; else [(R)+4, (R)+6] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: cleared
//  C: cleared
//
// Multiplies the A Argument by the B Argument and stores the result in the
// A Argument position on the stack.
//
// A <- A * B
//
void KD11CPU::FMUL (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    FISInstruction fisInstruction (cpu, instruction);
    executeFISinstruction (fisInstruction.getRegister (),
        [] (Float f1, Float f2) {return true;},
        [] (Float f1, Float f2) {return f1 * f2;});
}

// FDIV - floating divide
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6] / [(R),(R)+2 ]
//  if result >= 2^-128; else [(R)+4, (R)+6)] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: cleared
//  C: cleared
//
// Divides the A Argument by the B Argument and stores the result in the
// A Argument position on the stack. If the divisor (B Argument) is equal to
// zero, the stack is left untouched.
//
// A <- A / B
//
void KD11CPU::FDIV (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    FISInstruction fisInstruction (cpu, instruction);
    executeFISinstruction (fisInstruction.getRegister (),
        [] (Float f1, Float f2) {return f2.value() != 0;},
        [] (Float f1, Float f2) {return f1 / f2;});
}

// Pop a word from the processor stack returning true if this succeeds
// or false when a bus error occurs.
bool KD11CPU::popWord (u16 *destination)
{
    CondData<u16> tmpValue = fetchWord (register_[6]);
    *destination = tmpValue.valueOr (0);
    register_[6] += 2;
    if (!tmpValue.hasValue ())
        return false;
    return true;
}

// RTS - Return from subroutine
// 
// Operation:
//  PC <- (reg)
//  (reg) <- (SP)^
// 
// Loads contents of reg into PC and pops the top element of
// the processor stack into the specified register.
//
void KD11CPU::RTS (KD11CPU* cpu, u16 (&reg)[8], u16 instruction)
{
    FISInstruction rtsInstruction (cpu, instruction);
    u16 regNr = rtsInstruction.getRegister ();

    register_[7] = register_[regNr];
    popWord (&register_[regNr]);
}

#include "fisinstruction.h"

FISInstruction::FISInstruction (CpuData* cpu, u16 instruction)
    :
    LSI11Instruction (cpu),
    instr_ {instruction}
{}

u16 FISInstruction::getRegister ()
{
    return instr_.decoded.rn;
}

u16 FISInstruction::getOperationCode ()
{
    return instr_.decoded.opcode;
}

// Return the result of a floating point calculation
bool FISInstruction::returnFISresult (Float result, u16 registerNumber)
{
    u16 high, low;
    Float::Result conversionResult = result.pdp11Dword (&high, &low);
    if (conversionResult == Float::Result::OK)
    {
        cpu_->putWord (cpu_->registers ()[registerNumber] + 4, high);
        cpu_->putWord (cpu_->registers ()[registerNumber] + 6, low);
        cpu_->registers ()[registerNumber] += 4;

        setPSW (ConditionCodes {.N = result.value () < 0,
            .Z = result.value () == 0,
            .V = false,
            .C = false});
        return true;
    }

    if (conversionResult == Float::Result::Underflow)
    {
        setPSW (ConditionCodes {.N = true,
            .Z = false,
            .V = true,
            .C = false});

        cpu_->setTrap (CpuData::TrapCondition::FIS);
        return true;
    }
    
    // Overflow or Nan
    setPSW (ConditionCodes {.N = false,
        .Z = false,
        .V = true,
        .C = false});

    cpu_->setTrap (CpuData::TrapCondition::FIS);
    return true;
}

// Execute a FADD, FSUB, FMUL or FDIV instruction.
bool FISInstruction::executeFISinstruction (u16 stackPointer,
    std::function<bool(Float, Float)> argumentsValid,
    std::function<Float(Float, Float)> instruction)
{
    // Clear PSW bits 5 and 6
    cpu_->psw () &= ~(_BV(5) | _BV(6));

    CondData<u16> f1High = 
        cpu_->fetchWord (cpu_->registers ()[stackPointer] + 4);
    CondData<u16> f1Low = 
        cpu_->fetchWord (cpu_->registers ()[stackPointer] + 6);
    CondData<u16> f2High = 
        cpu_->fetchWord (cpu_->registers ()[stackPointer]);
    CondData<u16> f2Low = 
        cpu_->fetchWord (cpu_->registers ()[stackPointer] + 2);

    if (!f1High.hasValue () || !f1Low.hasValue () ||
        !f2High.hasValue () || !f2Low.hasValue ())
    {
        cpu_->setTrap (CpuData::TrapCondition::BusError);
        return true;
    }
     
    Float f1 (f1High, f1Low);
    Float f2 (f2High, f2Low);

    if (argumentsValid (f1, f2))
    {
        Float f3 = instruction (f1, f2);
        return returnFISresult (f3, stackPointer);
    }

    // The arguments are invalid. This is notably a division
    // by zero
    setPSW (ConditionCodes {.N = true,
        .Z = false,
        .V = true,
        .C = true});

    cpu_->setTrap (CpuData::TrapCondition::FIS);
    return true;
}
#include "fisinstruction.h"

FISInstruction::FISInstruction (CpuData* cpu, u16 instruction)
    :
    LSI11Instruction (cpu),
    instr_ {instruction},
	cpu_ {cpu}
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
CpuData::Trap FISInstruction::returnFISresult (Float result, u16 registerNumber)
{
    u16 high, low;
    Float::Result conversionResult = result.pdp11Dword (&high, &low);
    if (conversionResult == Float::Result::OK)
    {
        cpu_->putWord (cpu_->registers ()[registerNumber] + 4, high);
        cpu_->putWord (cpu_->registers ()[registerNumber] + 6, low);
        cpu_->registers ()[registerNumber] += 4;
        setConditionCodeIf_ClearElse (PSW_N, result.value () < 0);
        setConditionCodeIf_ClearElse (PSW_Z, result.value () == 0);
        clearConditionCode (PSW_V);
        clearConditionCode (PSW_C);
        return CpuData::Trap::None;
    }

    if (conversionResult == Float::Result::Underflow)
    {
        setConditionCode (PSW_N);
        setConditionCode (PSW_V);
        clearConditionCode (PSW_Z);
        clearConditionCode (PSW_C);
        return CpuData::Trap::FIS;
    }
    
    // Overflow or Nan
    clearConditionCode (PSW_N);
    setConditionCode (PSW_V);
    clearConditionCode (PSW_Z);
    clearConditionCode (PSW_C);
    return CpuData::Trap::FIS;
}

// Execute a FADD, FSUB, FMUL or FDIV instruction.
CpuData::Trap FISInstruction::executeFISinstruction (u16 stackPointer,
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
        return CpuData::Trap::BusError;
     
    Float f1 (f1High, f1Low);
    Float f2 (f2High, f2Low);

    if (argumentsValid (f1, f2))
    {
        Float f3 = instruction (f1, f2);
        return returnFISresult (f3, stackPointer);
    }

    // The arguments are invalid. This is notably a division
    // by zero
    setConditionCode (PSW_N);
    clearConditionCode (PSW_Z);
    setConditionCode (PSW_V);
    setConditionCode (PSW_C);
    return CpuData::Trap::FIS;
}
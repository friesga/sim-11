#include "executor.h"

KD11_NA::Executor::Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
    :
    commonExecutor (cpuData, cpuControl, mmu),
    cpuData_ {cpuData},
    mmu_ {mmu}
{}

// Return the result of a floating point calculation
bool KD11_NA::Executor::returnFISresult (Float result, u16 registerNumber)
{
    u16 high, low;
    Float::Result conversionResult = result.pdp11Dword (&high, &low);
    if (conversionResult == Float::Result::OK)
    {
        mmu_->putWord (cpuData_->registers ()[registerNumber] + 4, high);
        mmu_->putWord (cpuData_->registers ()[registerNumber] + 6, low);
        cpuData_->registers ()[registerNumber] += 4;

        commonExecutor.setPSW (ConditionCodes {.N = result.value () < 0,
            .Z = result.value () == 0,
            .V = false,
            .C = false});
        return true;
    }

    if (conversionResult == Float::Result::Underflow)
    {
        commonExecutor.setPSW (ConditionCodes {.N = true,
            .Z = false,
            .V = true,
            .C = false});

        cpuData_->setTrap (CpuData::TrapCondition::FIS);
        return true;
    }
    
    // Overflow or Nan
    commonExecutor.setPSW (ConditionCodes {.N = false,
        .Z = false,
        .V = true,
        .C = false});

    cpuData_->setTrap (CpuData::TrapCondition::FIS);
    return true;
}

// Execute a FADD, FSUB, FMUL or FDIV instruction.
bool KD11_NA::Executor::executeFISinstruction (u16 stackPointer,
    std::function<bool(Float, Float)> argumentsValid,
    std::function<Float(Float, Float)> instruction)
{
    // Clear PSW bits 5 and 6. This is necessary to succesfully execute
    // the VKACC1 diagnostic, in which the lower byte of the PSW is compared
    // with the expected result. Bits 5 and 6 of the PSW form the two
    // least significant bits of the priority level but are unused in the
    // KD11-NA.
    cpuData_->psw ().set (PSW::ProtectionMode::ExplicitAccess,
        cpuData_->psw () & ~(_BV(5) | _BV(6)));

    CondData<u16> f1High = 
        mmu_->fetchWord (cpuData_->registers ()[stackPointer] + 4);
    CondData<u16> f1Low = 
        mmu_->fetchWord (cpuData_->registers ()[stackPointer] + 6);
    CondData<u16> f2High = 
        mmu_->fetchWord (cpuData_->registers ()[stackPointer]);
    CondData<u16> f2Low = 
        mmu_->fetchWord (cpuData_->registers ()[stackPointer] + 2);

    if (!f1High.hasValue () || !f1Low.hasValue () ||
        !f2High.hasValue () || !f2Low.hasValue ())
    {
        cpuData_->setTrap (CpuData::TrapCondition::BusError);
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
    commonExecutor.setPSW (ConditionCodes {.N = true,
        .Z = false,
        .V = true,
        .C = true});

    cpuData_->setTrap (CpuData::TrapCondition::FIS);
    return true;
}
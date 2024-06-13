#include "executor.h"

KD11_NA::Executor::Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
    :
    commonExecutor (cpuData, cpuControl, mmu),
    cpuData_ {cpuData}
{}
 
 // Single operand instructions
bool KD11_NA::Executor::operator() (CLR& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (CLRB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (COM& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (COMB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (INC& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (INCB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (DEC& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (DECB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (NEG& instr)
{
    return commonExecutor.operator() (instr);
}
bool KD11_NA::Executor::operator() (NEGB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (TST& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (TSTB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (ASR& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (ASRB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (ASL& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (ASLB& instr)
{
    return commonExecutor.operator() (instr);;
}

bool KD11_NA::Executor::operator() (ROR& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (RORB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (ROL& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (ROLB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (SWAB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (ADC& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (ADCB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (SBC& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (SBCB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (SXT& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (MFPS& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (MTPS& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (JMP& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (MARK& instr)
{
    return commonExecutor.operator() (instr);
}

	// Double operand instructions
bool KD11_NA::Executor::operator() (MOV& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (MOVB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (CMP& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (CMPB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (ADD& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (SUB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BIT& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BITB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BIC& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BICB& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BIS& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BISB& instr)
{
    return commonExecutor.operator() (instr);
}

// EIS instructions, including JSR and XOR
bool KD11_NA::Executor::operator() (JSR& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (MUL& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (DIV& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (ASH& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (ASHC& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (XOR& instr)
{
    return commonExecutor.operator() (instr);
}


// SOB instruction format
bool KD11_NA::Executor::operator() (SOB& instr)
{
    return commonExecutor.operator() (instr);
}

// FIS format instructions
bool KD11_NA::Executor::operator() (RTS& instr)
{
    return commonExecutor.operator() (instr);
}

// The FIS instructions are specific for the KDF11-NA.
bool KD11_NA::Executor::operator() (FADD& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 + f2; });
}

bool KD11_NA::Executor::operator() (FSUB& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 - f2; });
}

bool KD11_NA::Executor::operator() (FMUL& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 * f2; });
}

bool KD11_NA::Executor::operator() (FDIV& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return f2.value () != 0; },
        [](Float f1, Float f2) { return f1 / f2; });
}

// Branch instructions
bool KD11_NA::Executor::operator() (BR& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BNE& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BEQ& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BPL& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BMI& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BVC& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BVS& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BCC& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BCS& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BGE& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BLT& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BGT& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BLE& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BHI& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BLOS& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (EMT& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (TRAP& instr)
{
    return commonExecutor.operator() (instr);
}

// No-operand instructions
bool KD11_NA::Executor::operator() (HALT& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (WAIT& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (RTI& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (BPT& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (IOT& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (RESET& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (RTT& instr)
{
    return commonExecutor.operator() (instr);
}

// Condition Code instructions
bool KD11_NA::Executor::operator() (CCC& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (SCC& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (Unused& instr)
{
    return commonExecutor.operator() (instr);
}

bool KD11_NA::Executor::operator() (MFPD& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
    return true;
}

bool KD11_NA::Executor::operator() (MTPD& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
    return true;
}

bool KD11_NA::Executor::operator() (MFPT& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
    return true;
}

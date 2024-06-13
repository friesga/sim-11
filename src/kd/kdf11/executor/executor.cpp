#include "executor.h"

KDF11_A::Executor::Executor::Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
    :
    Common::Executor (cpuData, cpuControl, mmu)
{}

#ifdef KDF11_A_FUNCTIONS 
bool KDF11_A::Executor::operator() (CLR& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (CLRB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (COM& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (COMB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (INC& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (INCB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (DEC& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (DECB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (NEG& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (NEGB& instr) { return instr.execute (); }

bool KDF11_A::Executor::operator() (ASR& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (ASRB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (ASL& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (ASLB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (ROR& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (RORB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (ROL& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (ROLB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (SWAB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (ADC& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (ADCB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (SBC& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (SBCB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (SXT& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (MFPS& instr) { return instr.execute (); }

	// Double operand instructions
bool KDF11_A::Executor::operator() (MOV& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (MOVB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (CMP& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (CMPB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (ADD& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (SUB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (BIT& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (BITB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (BIC& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (BICB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (BIS& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (BISB& instr) { return instr.execute (); }

	// EIS instructions, including JSR and XOR
bool KDF11_A::Executor::operator() (XOR& instr) { return instr.execute (); }

	// No-operand instructions
bool KDF11_A::Executor::operator() (HALT& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (WAIT& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (RESET& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (MFPD& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (MTPD& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (MFPT& instr) { return instr.execute (); }

// Unsupoorted instructions
bool KDF11_A::Executor::operator() (FADD& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (FSUB& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (FMUL& instr) { return instr.execute (); }
bool KDF11_A::Executor::operator() (FDIV& instr) { return instr.execute (); }
#else

 // Single operand instructions

bool KDF11_A::Executor::operator() (CLR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (CLRB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (COM& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (COMB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (INC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (INCB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (DEC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (DECB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (NEG& instr)
{
    return Common::Executor::operator() (instr);
}
bool KDF11_A::Executor::operator() (NEGB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (TST& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (TSTB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (ASR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (ASRB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (ASL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (ASLB& instr)
{
    return Common::Executor::operator() (instr);;
}

bool KDF11_A::Executor::operator() (ROR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (RORB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (ROL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (ROLB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (SWAB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (ADC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (ADCB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (SBC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (SBCB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (SXT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (MFPS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (MTPS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (JMP& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (MARK& instr)
{
    return Common::Executor::operator() (instr);
}

	// Double operand instructions
bool KDF11_A::Executor::operator() (MOV& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (MOVB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (CMP& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (CMPB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (ADD& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (SUB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BIT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BITB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BIC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BICB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BIS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BISB& instr)
{
    return Common::Executor::operator() (instr);
}

// EIS instructions, including JSR and XOR
bool KDF11_A::Executor::operator() (JSR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (MUL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (DIV& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (ASH& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (ASHC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (XOR& instr)
{
    return Common::Executor::operator() (instr);
}


// SOB instruction format
bool KDF11_A::Executor::operator() (SOB& instr)
{
    return Common::Executor::operator() (instr);
}

// FIS format instructions
bool KDF11_A::Executor::operator() (RTS& instr)
{
    return Common::Executor::operator() (instr);
}

// The FIS instructions are specific for the KDF11-NA.
bool KDF11_A::Executor::operator() (FADD& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 + f2; });
}

bool KDF11_A::Executor::operator() (FSUB& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 - f2; });
}

bool KDF11_A::Executor::operator() (FMUL& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 * f2; });
}

bool KDF11_A::Executor::operator() (FDIV& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return f2.value () != 0; },
        [](Float f1, Float f2) { return f1 / f2; });
}

// Branch instructions
bool KDF11_A::Executor::operator() (BR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BNE& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BEQ& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BPL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BMI& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BVC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BVS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BCC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BCS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BGE& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BLT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BGT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BLE& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BHI& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BLOS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (EMT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (TRAP& instr)
{
    return Common::Executor::operator() (instr);
}

// No-operand instructions
bool KDF11_A::Executor::operator() (HALT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (WAIT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (RTI& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (BPT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (IOT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (RESET& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (RTT& instr)
{
    return Common::Executor::operator() (instr);
}

// Condition Code instructions
bool KDF11_A::Executor::operator() (CCC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (SCC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (Unused& instr)
{
    return Common::Executor::operator() (instr);
}

bool KDF11_A::Executor::operator() (MFPD& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
    return true;
}

bool KDF11_A::Executor::operator() (MTPD& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
    return true;
}

bool KDF11_A::Executor::operator() (MFPT& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
    return true;
}


#endif // KDF11_A_FUNCTIONS
#include "executor.h"

#include "kd/common/instructions/fadd.h"
#include "kd/common/instructions/fdiv.h"
#include "kd/common/instructions/fmul.h"
#include "kd/common/instructions/fsub.h"

KD11_NA::Executor::Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
    :
    Common::Executor (cpuData, cpuControl, mmu)
{}
 
 // Single operand instructions
bool KD11_NA::Executor::operator() (CLR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CLRB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (COM& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (COMB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (INC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (INCB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (DEC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (DECB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (NEG& instr)
{
    return Common::Executor::operator() (instr);
}
bool KD11_NA::Executor::operator() (NEGB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (TST& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (TSTB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (ASR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (ASRB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (ASL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (ASLB& instr)
{
    return Common::Executor::operator() (instr);;
}

bool KD11_NA::Executor::operator() (ROR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (RORB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (ROL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (ROLB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (SWAB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (ADC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (ADCB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (SBC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (SBCB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (SXT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (MFPS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (MTPS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (JMP& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (MARK& instr)
{
    return Common::Executor::operator() (instr);
}

	// Double operand instructions
bool KD11_NA::Executor::operator() (MOV& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (MOVB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CMP& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CMPB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (ADD& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (SUB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BIT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BITB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BIC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BICB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BIS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BISB& instr)
{
    return Common::Executor::operator() (instr);
}

// EIS instructions, including JSR and XOR
bool KD11_NA::Executor::operator() (JSR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (MUL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (DIV& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (ASH& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (ASHC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (XOR& instr)
{
    return Common::Executor::operator() (instr);
}


// SOB instruction format
bool KD11_NA::Executor::operator() (SOB& instr)
{
    return Common::Executor::operator() (instr);
}

// FIS format instructions
bool KD11_NA::Executor::operator() (RTS& instr)
{
    return Common::Executor::operator() (instr);
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
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BNE& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BEQ& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BPL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BMI& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BVC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BVS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BCC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BCS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BGE& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BLT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BGT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BLE& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BHI& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BLOS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (EMT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (TRAP& instr)
{
    return Common::Executor::operator() (instr);
}

// No-operand instructions
bool KD11_NA::Executor::operator() (HALT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (WAIT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (RTI& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (BPT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (IOT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (RESET& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (RTT& instr)
{
    return Common::Executor::operator() (instr);
}

// Condition Code instructions
bool KD11_NA::Executor::operator() (CCC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (SCC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (Unused& instr)
{
    return Common::Executor::operator() (instr);
}

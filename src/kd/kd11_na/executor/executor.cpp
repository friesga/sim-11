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
bool KD11_NA::Executor::operator() (CommonInstruction::CLR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::CLRB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::COM& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::COMB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::INC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::INCB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::DEC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::DECB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::NEG& instr)
{
    return Common::Executor::operator() (instr);
}
bool KD11_NA::Executor::operator() (CommonInstruction::NEGB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::TST& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::TSTB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::ASR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::ASRB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::ASL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::ASLB& instr)
{
    return Common::Executor::operator() (instr);;
}

bool KD11_NA::Executor::operator() (CommonInstruction::ROR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::RORB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::ROL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::ROLB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::SWAB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::ADC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::ADCB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::SBC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::SBCB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::SXT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::MFPS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::MTPS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::JMP& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::MARK& instr)
{
    return Common::Executor::operator() (instr);
}

	// Double operand instructions
bool KD11_NA::Executor::operator() (CommonInstruction::MOV& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::MOVB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::CMP& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::CMPB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::ADD& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::SUB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BIT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BITB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BIC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BICB& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BIS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BISB& instr)
{
    return Common::Executor::operator() (instr);
}

// EIS instructions, including JSR and XOR
bool KD11_NA::Executor::operator() (CommonInstruction::JSR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::MUL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::DIV& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::ASH& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::ASHC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::XOR& instr)
{
    return Common::Executor::operator() (instr);
}


// SOB instruction format
bool KD11_NA::Executor::operator() (CommonInstruction::SOB& instr)
{
    return Common::Executor::operator() (instr);
}

// FIS format instructions
bool KD11_NA::Executor::operator() (CommonInstruction::RTS& instr)
{
    return Common::Executor::operator() (instr);
}

// The FIS instructions are specific for the KDF11-NA.
bool KD11_NA::Executor::operator() (CommonInstruction::FADD& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 + f2; });
}

bool KD11_NA::Executor::operator() (CommonInstruction::FSUB& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 - f2; });
}

bool KD11_NA::Executor::operator() (CommonInstruction::FMUL& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 * f2; });
}

bool KD11_NA::Executor::operator() (CommonInstruction::FDIV& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return f2.value () != 0; },
        [](Float f1, Float f2) { return f1 / f2; });
}

// Branch instructions
bool KD11_NA::Executor::operator() (CommonInstruction::BR& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BNE& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BEQ& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BPL& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BMI& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BVC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BVS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BCC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BCS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BGE& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BLT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BGT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BLE& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BHI& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BLOS& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::EMT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::TRAP& instr)
{
    return Common::Executor::operator() (instr);
}

// No-operand instructions
bool KD11_NA::Executor::operator() (CommonInstruction::HALT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::WAIT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::RTI& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::BPT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::IOT& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::RESET& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::RTT& instr)
{
    return Common::Executor::operator() (instr);
}

// Condition Code instructions
bool KD11_NA::Executor::operator() (CommonInstruction::CCC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::SCC& instr)
{
    return Common::Executor::operator() (instr);
}

bool KD11_NA::Executor::operator() (CommonInstruction::Unused& instr)
{
    return Common::Executor::operator() (instr);
}

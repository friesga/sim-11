#ifndef _KD11_NA_EXECUTOR_H_
#define _KD11_NA_EXECUTOR_H_

#include "kd/common/executor/executor.h"

namespace KD11_NA {

// This class contains the KD11-NA specific execution of some instructions.
// For most instructions the execution is forwarded to the Common::Executor.
//
class Executor
{
public:
    Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu);

	// The operator() function is specialized for the instructions specific
    // for the KD11-NA. The definition of these specializations is rather
    // peculiar. They cannot be declared in the class definition but have to
    // be declared and defined outside of the class in the same .h file with
    // the inline specifier.
	template <typename T>
	bool operator() (T& instr);

private:
	Common::Executor commonExecutor;
	CpuData* cpuData_;
};

template <typename T>
bool Executor::operator() (T& instr)
{
	return commonExecutor.execute (instr);
}

// Definition of the instructions specific for the KD11-nA.
template <>
inline bool Executor::operator()<FADD> (FADD& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 + f2; });
}

template <>
inline bool Executor::operator()<FSUB> (FSUB& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 - f2; });
}

template <>
inline bool Executor::operator()<FMUL> (FMUL& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 * f2; });
}

template <>
inline bool Executor::operator()<FDIV> (FDIV& instr)
{
    return instr.executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return f2.value () != 0; },
        [](Float f1, Float f2) { return f1 / f2; });
}

template <>
inline bool Executor::operator()<MFPD> (MFPD& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
    return true;
}

template <>
inline bool Executor::operator()<MTPD> (MTPD& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
    return true;
}

template <>
inline bool Executor::operator()<MFPT> (MFPT& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
    return true;
}
 
} // namespace KD11_NA

#endif // _KD11_NA_EXECUTOR_H_
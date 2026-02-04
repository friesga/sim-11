#ifndef _KD11_NA_EXECUTOR_H_
#define _KD11_NA_EXECUTOR_H_

#include "proc/common/executor/executor.h"

// This class contains the KD11-NA specific execution of some instructions.
// For most instructions the execution is forwarded to the Common::Executor.
//
class KD11_NA_Executor
{
public:
    KD11_NA_Executor (CpuData* cpuData,
        Interfaces::CpuController* cpuController, DataPaths* dataPaths);

	// The operator() function is specialized for the instructions specific
    // for the KD11-NA. The definition of these specializations is rather
    // peculiar. They cannot be declared in the class definition but have to
    // be declared and defined outside of the class in the same .h file with
    // the inline specifier.
	template <typename T>
	bool operator() (T& instr);

private:
	Common::Executor commonExecutor_;
	CpuData* cpuData_;
    DataPaths* dataPaths_;

    bool returnFISresult (Float result, u16 registerNumber);
    bool executeFISinstruction (u16 stackPointer,
        std::function<bool (Float, Float)> argumentsValid,
        std::function<Float (Float, Float)> instruction);
};

template <typename T>
bool KD11_NA_Executor::operator() (T& instr)
{
    if constexpr (requires {commonExecutor_.template execute<WriteOperandOrder::WriteOperandBeforeCC> (instr); })
        return commonExecutor_.template execute<WriteOperandOrder::WriteOperandBeforeCC> (instr);
    else
        return commonExecutor_.execute (instr);
}

// Definition of the instructions specific for the KD11-nA.
template <>
inline bool KD11_NA_Executor::operator()<FADD> (FADD& instr)
{
    return executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 + f2; });
}

template <>
inline bool KD11_NA_Executor::operator()<FSUB> (FSUB& instr)
{
    return executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 - f2; });
}

template <>
inline bool KD11_NA_Executor::operator()<FMUL> (FMUL& instr)
{
    return executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 * f2; });
}

template <>
inline bool KD11_NA_Executor::operator()<FDIV> (FDIV& instr)
{
    return executeFISinstruction (instr.getRegister (),
        [](Float f1, Float f2) { return f2.value () != 0; },
        [](Float f1, Float f2) { return f1 / f2; });
}

// Instructions not implemented on the KD11-NA.
template <>
inline bool KD11_NA_Executor::operator()<MFPD> (MFPD& instr)
{
    cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
    return true;
}

template <>
inline bool KD11_NA_Executor::operator()<MTPD> (MTPD& instr)
{
    cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
    return true;
}

// Since the KDF11-A doesn't support separate I/D space the behaviour of the
// MFPD and MTPD on this processor equals to that of the MFPI and MTPI
// instructions.
template <>
inline bool KD11_NA_Executor::operator()<MFPI> (MFPI& instr)
{
    cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
    return true;
}

template <>
inline bool KD11_NA_Executor::operator()<MTPI> (MTPI& instr)
{
    cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
    return true;
}

template <>
inline bool KD11_NA_Executor::operator()<MFPT> (MFPT& instr)
{
    cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
    return true;
}
 
#endif // _KD11_NA_EXECUTOR_H_
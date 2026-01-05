#ifndef _KA11_EXECUTOR_H_
#define _KA11_EXECUTOR_H_

#include "proc/common/executor/executor.h"

// Definition of the instructions specific for the KA11.
//
template <typename T>
concept UnsupportedInstruction =
std::same_as<T, SXT>  ||
std::same_as<T, MFPS> ||
std::same_as<T, MTPS> ||
std::same_as<T, MARK> ||
std::same_as<T, MUL>  ||
std::same_as<T, DIV>  ||
std::same_as<T, ASH>  ||
std::same_as<T, ASHC> ||
std::same_as<T, XOR>  ||
std::same_as<T, SOB>  ||
std::same_as<T, FADD> ||
std::same_as<T, FSUB> ||
std::same_as<T, FMUL> ||
std::same_as<T, FDIV> ||
std::same_as<T, BPT>  ||
std::same_as<T, RTT>  ||
std::same_as<T, CCC>  ||
std::same_as<T, SCC>  ||
std::same_as<T, MFPD> ||
std::same_as<T, MTPD> ||
std::same_as<T, MFPI> ||
std::same_as<T, MTPI> ||
std::same_as<T, MFPT>;


// This class contains the KA11 specific execution of some instructions.
// For most instructions the execution is forwarded to the Common::Executor.
//
class KA11_Executor
{
public:
    KA11_Executor (CpuData* cpuData, Interfaces::CpuController* cpuController, MMU* mmu);

	// The operator() function is specialized for the instructions specific
    // for the KA11. The definition of these specializations is rather
    // peculiar. They cannot be declared in the class definition but have to
    // be declared and defined outside of the class in the same .h file with
    // the inline specifier.
	template <typename T>
	bool operator() (T& instr);

    template <UnsupportedInstruction T>
    bool operator() (T& instr);

private:
    Common::Executor commonExecutor_;
    CpuData* cpuData_;
};

template <typename T>
bool KA11_Executor::operator() (T& instr)
{
    if constexpr (requires {commonExecutor_.template execute<WriteOperandOrder::WriteOperandBeforeCC> (instr); })
        return commonExecutor_.template execute<WriteOperandOrder::WriteOperandBeforeCC> (instr);
    else
        return commonExecutor_.execute (instr);
}

template <UnsupportedInstruction T>
inline bool KA11_Executor::operator() (T& instr)
{
    cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
    return true;
}

#endif // _KA11_EXECUTOR_H_
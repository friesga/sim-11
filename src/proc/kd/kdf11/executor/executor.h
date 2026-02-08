#ifndef _KDF11_EXECUTOR_H_
#define _KDF11_EXECUTOR_H_

#include "proc/common/executor/executor.h"

// This class contains the KDF11-A specific execution of some instructions.
// For most instructions the execution is forwarded to the Common::Executor.
//
class KDF11_Executor
{
public:
    KDF11_Executor (CpuData* cpuData,
        Interfaces::CpuController* cpuController, DataPaths* dataPaths);

	template <typename T>
	bool operator() (T& instr);

private:
	Common::Executor commonExecutor_;
	CpuData* cpuData_;
    Interfaces::CpuController* cpuController_;
    DataPaths* dataPaths_;

    OperandDecoderFactory operandDecoderFactory_ {cpuData_, cpuController_, dataPaths_};
};

template <typename T>
bool KDF11_Executor::operator() (T& instr)
{
    if constexpr (requires {commonExecutor_.template execute<WriteOperandOrder::WriteOperandBeforeCC> (instr); })
        return commonExecutor_.template execute<WriteOperandOrder::WriteOperandAfterCC> (instr);
    else
        return commonExecutor_.execute (instr);
}

// No-operand instructions
template <>
inline bool KDF11_Executor::operator() (HALT& instr)
{
    if (cpuData_->psw ().currentMode () == PSW::Mode::User)
    {
        cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
        return false;
    }
    cpuController_->halt ();

    return true;
}

template <>
inline bool KDF11_Executor::operator() (WAIT& instr)
{
    if (!cpuData_->psw ().traceBitSet ())
        cpuController_->wait ();

    return true;
}

template <>
inline bool KDF11_Executor::operator() (RESET& instr)
{
    if (cpuData_->psw ().currentMode () != PSW::Mode::User)
        cpuController_->busReset ();

    return true;
}

template <>
inline bool KDF11_Executor::operator() (MFPD& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> source;

    // The source operand is determined in the current memory management
    // mode and then retrieved using the previous mode.
    OperandLocation operandLocation =  
        singleOperandDecoder->getOperandLocation (cpuData_->registers ());
    source = operandLocation.prevModeContents<CondData<u16>> ();

    if (!source.hasValue ())
        return false;

    if (!cpuController_->pushWord (source))
        return false;

    if (cpuData_->stackOverflow ())
        cpuData_->setTrap (CpuData::TrapType::StackOverflow);

    commonExecutor_.setPSW (ConditionCodes {.N = (bool) (source & 0100000),
        .Z = source == 0,
        .V = false});

    return true;
}

template <>
inline bool KDF11_Executor::operator() (MTPD& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    u16 tmp;
    
    // The destination operand and the value popped off the stack are 
    // retrieved in the current memory management and the tmp value then
    // is written using the previous mode.
    OperandLocation operandLocation =  
        singleOperandDecoder->getOperandLocation (cpuData_->registers ());

    if (!cpuController_->popWord (&tmp) || !operandLocation.writePrevMode (tmp))
        return false;
        
    commonExecutor_.setPSW (ConditionCodes {.N = (bool) (tmp & 0100000),
        .Z = tmp == 0,
        .V = false});

    return true;
}

template <>
inline bool KDF11_Executor::operator() (MFPI& instr)
{
    return this->operator() (reinterpret_cast<MFPD&> (instr));
}

template <>
inline bool KDF11_Executor::operator() (MTPI& instr)
{
    return this->operator() (reinterpret_cast<MTPD&> (instr));
}

template <>
inline bool KDF11_Executor::operator() (MFPT& instr)
{
    cpuData_->registers ()[0] = 3;
    return true;
}

// Unsupported instructions
template <>
inline bool KDF11_Executor::operator() (FADD& instr)
{
    cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
	return false;
}

template <>
inline bool KDF11_Executor::operator() (FSUB& instr)
{
    cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
	return false;
}

template <>
inline bool KDF11_Executor::operator() (FMUL& instr)
{
    cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
	return false;
}

template <>
inline bool KDF11_Executor::operator() (FDIV& instr)
{
    cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
	return false;
}

#endif // _KDF11_EXECUTOR_H_
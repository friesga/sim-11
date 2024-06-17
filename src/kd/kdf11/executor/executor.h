#ifndef _KDF11_A_EXECUTOR_H_
#define _KDF11_A_EXECUTOR_H_

#include "kd/common/executor/executor.h"

namespace KDF11_A {

// This class contains the KDF11-A specific execution of some instructions.
// For most instructions the execution is forwarded to the Common::Executor.
//
class Executor
{
public:
    Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu);

	template <typename T>
	bool operator() (T& instr);

private:
	Common::Executor commonExecutor;
	CpuData* cpuData_;
    CpuControl* cpuControl_;
    MMU* mmu_;
};

template <typename T>
bool Executor::operator() (T& instr)
{
	return commonExecutor.execute (instr);
}

// Single operand instructions
template <>
inline bool Executor::operator() (CLR& instr)
{
    commonExecutor.setPSW (ConditionCodes {.N = false,
        .Z = true,
        .V = false,
        .C = false});

    if (!instr.writeOperand ((u16) 0))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (CLRB& instr)
{
    commonExecutor.setPSW (ConditionCodes {.N = false,
        .Z = true,
        .V = false,
        .C = false});

    if (!instr.writeOperand ((u8) 0))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (COM& instr)
{
    CondData<u16> operand;
    if (!instr.readOperand (&operand))
        return false;

    // Complement the operand, set the condition codes accordingly and
    // write it to the operand location.
    operand = ~operand;

    commonExecutor.setPSW ({ConditionCodes {.N = (bool) (operand & 0x8000),
        .Z = operand == 0,
        .V = false,
        .C = true}});

    if (!instr.writeOperand (operand.value ()))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (COMB& instr)
{
    CondData<u8> operand;
    if (!instr.readOperand (&operand))
        return false;

    // Complement the operand, adjust the condition codes and write the
    // complement to the operand location.
    operand = ~operand;

    commonExecutor.setPSW ({ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = (u8) operand == 0,
        .V = false,
        .C = true}});

    if (!instr.writeOperand (operand.value ()))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (INC& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    // Increment the operand and write it to the operand location
    u16 result = contents + 1;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 077777});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (INCB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u8 result = (u8)(source + 1);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = source == 000177});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (DEC& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    // Increment the operand and write it to the operand location
    u16 result = contents - 1;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = contents == 0100000});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (DECB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u8 result = (u8) (source - 1);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = source == 0000200});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (NEG& instr)
{
    CondData<u16> operand;
    if (!instr.readOperand (&operand))
        return false;

    // Negate the operand, adjust the condition codes accordingly and write
    // it the negated operand to the operand location.
    if (operand != 0100000)
        operand = -operand;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (operand & 0100000),
        .Z = operand == 0,
        .V = operand == 0100000,
        .C = operand != 0});

    if (!instr.writeOperand (operand.value ()))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (NEGB& instr)
{
    CondData<u8> operand;
    if (!instr.readOperand (&operand))
        return false;

    if (operand != 0200)
        operand = -operand;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = operand == 0,
        .V = operand == 0200,
        .C = operand != 0});

    if (!instr.writeOperand (operand.value ()))
        return false;

    return true;
}


template <>
inline bool Executor::operator() (ASR& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    u16 result = contents;
    if (result & 0100000)
    {
        result >>= 1;
        result |= 0100000;
    }
    else
        result >>= 1;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = (bool) (result & 0100000) != (bool) (contents & 1),
        .C = (bool) (contents & 1)});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (ASRB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u8 result = source;
    if (result & 0x80)
    {
        result >>= 1;
        result |= 0x80;
    }
    else
        result >>= 1;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) (result & 0x80) != (bool) (source & 1),
        .C = (bool) (source & 1)});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (ASL& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    u16 result = contents << 1;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = ((bool) (result & 0100000) != (bool) (contents & 0100000)),
        .C = (bool) (contents & 0100000)});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (ASLB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u8 result = (u8) (source << 1);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) (result & 0x80) != (bool) (source & 0x80),
        .C = (bool) (source & 0x80)});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (ROR& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    u16 result = contents >> 1;
    if (commonExecutor.isSet (PSW_C))
        result |= 0100000;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = (bool) (result & 0100000) != (bool) (contents & 0000001),
        .C = (bool) (contents & 0000001)});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (RORB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u8 result = source >> 1;
    if (commonExecutor.isSet (PSW_C))
        result |= 0x80;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) (result & 0x80) != (bool) (source & 0x01),
        .C = (bool) (source & 0x01)});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (ROL& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    u16 cBit = commonExecutor.isSet (PSW_C);
    u16 result = contents << 1;
    if (cBit)
        result |= 01;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = (bool) (result & 0100000) != (bool) (contents & 0100000),
        .C = (bool) (contents & 0100000)});


    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (ROLB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u8 result = (u8) (source << 1);
    if (commonExecutor.isSet (PSW_C))
        result |= 0x01;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) ((result & 0x80) != (source & 0x80)),
        .C = (bool) (source & 0x80)});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (SWAB& instr)
{
    CondData<u16> operand;
    if (!instr.readOperand (&operand))
        return false;

    // Swap bytes in the operand and write it to the operand location
    operand = ((operand & 0x00FF) << 8) | ((operand >> 8) & 0xFF);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = (u8) operand == 0,
        .V = false,
        .C = false});

    if (!instr.writeOperand (operand.value ()))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (ADC& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    u16 cBit = commonExecutor.isSet (PSW_C) ? 1 : 0;
    u16 result = contents + cBit;

    commonExecutor.setPSW (ConditionCodes ({.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 0077777 && commonExecutor.isSet (PSW_C),
        .C = contents == 0177777 && commonExecutor.isSet (PSW_C)}));

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (ADCB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u16 tmp = commonExecutor.isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8) (source + tmp);

    commonExecutor.setPSW (ConditionCodes ({.N = (bool) (destination & 0x80),
        .Z = destination == 0,
        .V = source == 0177 && commonExecutor.isSet (PSW_C),
        .C = source == 0377 && commonExecutor.isSet (PSW_C)}));

    if (!instr.writeOperand (destination))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (SBC& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    u16 cBit = commonExecutor.isSet (PSW_C) ? 1 : 0;
    u16 result = contents - cBit;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 0100000,
        .C = contents == 0 && cBit});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (SBCB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u16 cBit = commonExecutor.isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8) (source - cBit);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (destination & 0x80),
        .Z = destination == 0,
        .V = source == 0200,
        .C = source == 0 && cBit});

    if (!instr.writeOperand (destination))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (SXT& instr)
{
    u16 result = commonExecutor.isSet (PSW_N) ? 0177777 : 0;

    commonExecutor.setPSW (ConditionCodes {.Z = !commonExecutor.isSet (PSW_N),
        .V = false});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (MFPS& instr)
{
    u8 contents = (u8) cpuData_->psw ();
    OperandLocation operandLocation_ = 
        instr.getOperandLocation (cpuData_->registers ());

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (contents & 0x80),
        .Z = (contents & 0xFF) == 0,
        .V = false});

    if (operandLocation_.isA<RegisterOperandLocation> ())
    {
        // If destination is mode 0 (Register), the regular operand processing
        // is bypassed and PS bit 7 is sign extended through the upper byte of
        // the register.
        cpuData_->registers ()[operandLocation_] = (s8) contents;
    }
    else
    {
        if (!instr.writeOperand (contents))
            return false;
    }
    
    return true;
}

// Double operand instructions
template <>
inline bool Executor::operator() (MOV& instr)
{
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source))
        return false;

    commonExecutor.setPSW (ConditionCodes ({.N = (bool) (source & 0100000),
        .Z = source == 0,
        .V = false}));
    
    if (!instr.writeDestinationOperand (source.value ()))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (MOVB& instr)
{
    CondData<u8> source;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source))
        return false;

    // Make the source a signed eight bit value 
    s8 tmp = (s8) source;

    commonExecutor.setPSW (ConditionCodes ({.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false}));

    // Using the overloaded MOVB::writeDestinationOperand()
    if (!instr.writeDestinationOperands8 (tmp))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (CMP& instr)
{
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source) ||
        !instr.readDestinationOperand (&destination))
        return false;

    u16 tmp = source - destination;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = ((source & 0x8000) != (destination & 0x8000)) && ((destination & 0x8000) == (tmp & 0x8000)),
        .C = (bool) (((u32) source - (u32) destination) & 0x10000)});

    return true;
}

template <>
inline bool Executor::operator() (CMPB& instr)
{
    CondData<u8> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source) || 
            !instr.readDestinationOperand (&destination))
        return false;

    u16 tmp = (u8) (source - destination);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = ((source & 0x80) != (destination & 0x80)) && ((destination & 0x80) == (tmp & 0x80)),
        .C = (bool) ((source - destination) & 0x100)});

    return true;
}

template <>
inline bool Executor::operator() (ADD& instr)
{
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source) ||
            !instr.readDestinationOperand (&destination))
        return false;

    u16 result = source + destination;

    commonExecutor.setPSW (ConditionCodes ({.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = ((source & 0x8000) == (destination & 0x8000)) &&
            ((destination & 0x8000) != (result & 0x8000)),
        .C = (bool) (((u32) source + (u32) destination) & 0x10000)}));

    if (!instr.writeDestinationOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (SUB& instr)
{
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source) ||
            !instr.readDestinationOperand (&destination))
        return false;

    u16 result = destination - source;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = ((source & 0x8000) != (destination & 0x8000)) &&
             ((source & 0x8000) == (result & 0x8000)),
        .C = (bool) (((u32) destination - (u32) source) & 0x10000)});

    if (!instr.writeDestinationOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (BIT& instr)
{
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source) ||
            !instr.readDestinationOperand (&destination))
        return false;

    u16 tmp = source & destination;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = false});

    return true;
}

template <>
inline bool Executor::operator() (BITB& instr)
{
    CondData<u8> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source) || 
            !instr.readDestinationOperand (&destination))
        return false;

    u16 tmp = source & destination;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    return true;
}

template <>
inline bool Executor::operator() (BIC& instr)
{
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source) ||
            !instr.readDestinationOperand (&destination))
        return false;

    u16 result = ~source & destination;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = false});

    if (!instr.writeDestinationOperand (result))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (BICB& instr)
{
    CondData<u8> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source) || 
            !instr.readDestinationOperand (&destination))
        return false;

    u8 tmp = (u8)(~source & destination);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    if (!instr.writeDestinationOperand (tmp))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (BIS& instr)
{
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source) ||
            !instr.readDestinationOperand (&destination))
        return false;

    u16 tmp = source | destination;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = false});

    if (!instr.writeDestinationOperand (tmp))
        return false;

    return true;
}

template <>
inline bool Executor::operator() (BISB& instr)
{
    CondData<u8> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    instr.getOperandLocations (cpuData_->registers ());

    if (!instr.readSourceOperand (&source) || 
            !instr.readDestinationOperand (&destination))
        return false;

    u8 tmp = source | destination;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    if (!instr.writeDestinationOperand (tmp))
        return false;

    return true;
}

// EIS instructions, including JSR and XOR
template <>
inline bool Executor::operator() (XOR& instr)
{
    u16 regNr = instr.getRegisterNr ();
    GeneralRegisters& registers = cpuData_->registers ();

    u16 source = registers[regNr];

    CondData<u16> destination;
    if (!instr.readOperand (&destination))
        return false;

    u16 result = source ^ destination;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = false});

    if (!instr.writeOperand (result))
        return false;

    return true;
}

// No-operand instructions
template <>
inline bool Executor::operator() (HALT& instr)
{
    if (cpuData_->psw ().currentMode () == PSW::Mode::User)
    {
        cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
        return false;
    }
    cpuControl_->halt ();

    return true;
}

template <>
inline bool Executor::operator() (WAIT& instr)
{
    if (!cpuData_->psw ().traceBitSet ())
        cpuControl_->wait ();

    return true;
}

template <>
inline bool Executor::operator() (RESET& instr)
{
    if (cpuData_->psw ().currentMode () != PSW::Mode::User)
        cpuControl_->busReset ();

    return true;
}

template <>
inline bool Executor::operator() (MFPD& instr)
{
    CondData<u16> source;

    // The source operand is determined in the current memory management
    // mode and then retrieved using the previous mode.
    OperandLocation operandLocation_ =  
        instr.getOperandLocation (cpuData_->registers ());
    source = operandLocation_.prevModeContents<CondData<u16>> ();

    if (!source.hasValue ())
        return false;

    if (!mmu_->pushWord (source))
        return false;

    if (cpuData_->stackOverflow ())
        cpuData_->setTrap (CpuData::TrapCondition::StackOverflow);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (source & 0100000),
        .Z = source == 0,
        .V = false});

    return true;
}

template <>
inline bool Executor::operator() (MTPD& instr)
{
    u16 tmp;
    
    // The destination operand and the value popped off the stack are 
    // retrieved in the current memory management and the tmp value then
    // is written using the previous mode.
    OperandLocation operandLocation_ =  
        instr.getOperandLocation (cpuData_->registers ());

    if (!mmu_->popWord (&tmp) || !operandLocation_.writePrevMode (tmp))
        return false;
        
    commonExecutor.setPSW (ConditionCodes {.N = (bool) (tmp & 0100000),
        .Z = tmp == 0,
        .V = false});

    return true;
}

template <>
inline bool Executor::operator() (MFPT& instr)
{
    cpuData_->registers ()[0] = 3;
    return true;
}

// Unsupported instructions
template <>
inline bool Executor::operator() (FADD& instr)
{
	return false;
}

template <>
inline bool Executor::operator() (FSUB& instr)
{
	return false;
}

template <>
inline bool Executor::operator() (FMUL& instr)
{
	return false;
}

template <>
inline bool Executor::operator() (FDIV& instr)
{
	return false;
}

} // namespace KDF11_A

#endif // _KDF11_A_EXECUTOR_H_
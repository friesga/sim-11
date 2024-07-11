#ifndef _KDF11_A_EXECUTOR_H_
#define _KDF11_A_EXECUTOR_H_

#include "kd/common/executor/executor.h"

namespace KDF11 {

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

    OperandDecoderFactory operandDecoderFactory_ {cpuData_, cpuControl_, mmu_};
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
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);

    commonExecutor.setPSW (ConditionCodes {.N = false,
        .Z = true,
        .V = false,
        .C = false});

    return singleOperandDecoder->writeOperand ((u16) 0);
}

template <>
inline bool Executor::operator() (CLRB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);

    commonExecutor.setPSW (ConditionCodes {.N = false,
        .Z = true,
        .V = false,
        .C = false});

    return singleOperandDecoder->writeOperand ((u8) 0);
}

template <>
inline bool Executor::operator() (COM& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> operand;

    if (!singleOperandDecoder->readOperand (&operand))
        return false;

    // Complement the operand, set the condition codes accordingly and
    // write it to the operand location.
    operand = ~operand;

    commonExecutor.setPSW ({ConditionCodes {.N = (bool) (operand & 0x8000),
        .Z = operand == 0,
        .V = false,
        .C = true}});

    return singleOperandDecoder->writeOperand (operand.value ());
}

template <>
inline bool Executor::operator() (COMB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> operand;

    if (!singleOperandDecoder->readOperand (&operand))
        return false;

    // Complement the operand, adjust the condition codes and write the
    // complement to the operand location.
    operand = ~operand;

    commonExecutor.setPSW ({ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = (u8) operand == 0,
        .V = false,
        .C = true}});

    return singleOperandDecoder->writeOperand (operand.value ());
}

template <>
inline bool Executor::operator() (INC& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    // Increment the operand and write it to the operand location
    u16 result = contents + 1;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 077777});

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (INCB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u8 result = (u8)(source + 1);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = source == 000177});

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (DEC& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    // Increment the operand and write it to the operand location
    u16 result = contents - 1;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = contents == 0100000});

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (DECB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u8 result = (u8) (source - 1);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = source == 0000200});

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (NEG& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> operand;

    if (!singleOperandDecoder->readOperand (&operand))
        return false;

    // Negate the operand, adjust the condition codes accordingly and write
    // it the negated operand to the operand location.
    if (operand != 0100000)
        operand = -operand;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (operand & 0100000),
        .Z = operand == 0,
        .V = operand == 0100000,
        .C = operand != 0});

    return singleOperandDecoder->writeOperand (operand.value ());
}

template <>
inline bool Executor::operator() (NEGB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> operand;

    if (!singleOperandDecoder->readOperand (&operand))
        return false;

    if (operand != 0200)
        operand = -operand;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = operand == 0,
        .V = operand == 0200,
        .C = operand != 0});

    return singleOperandDecoder->writeOperand (operand.value ());
}


template <>
inline bool Executor::operator() (ASR& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
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

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (ASRB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
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

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (ASL& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    u16 result = contents << 1;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = ((bool) (result & 0100000) != (bool) (contents & 0100000)),
        .C = (bool) (contents & 0100000)});

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (ASLB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u8 result = (u8) (source << 1);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) (result & 0x80) != (bool) (source & 0x80),
        .C = (bool) (source & 0x80)});

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (ROR& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    u16 result = contents >> 1;
    if (commonExecutor.isSet (PSW_C))
        result |= 0100000;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = (bool) (result & 0100000) != (bool) (contents & 0000001),
        .C = (bool) (contents & 0000001)});

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (RORB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u8 result = source >> 1;
    if (commonExecutor.isSet (PSW_C))
        result |= 0x80;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) (result & 0x80) != (bool) (source & 0x01),
        .C = (bool) (source & 0x01)});

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (ROL& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    u16 cBit = commonExecutor.isSet (PSW_C);
    u16 result = contents << 1;
    if (cBit)
        result |= 01;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = (bool) (result & 0100000) != (bool) (contents & 0100000),
        .C = (bool) (contents & 0100000)});


    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (ROLB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u8 result = (u8) (source << 1);
    if (commonExecutor.isSet (PSW_C))
        result |= 0x01;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) ((result & 0x80) != (source & 0x80)),
        .C = (bool) (source & 0x80)});

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (SWAB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> operand;

    if (!singleOperandDecoder->readOperand (&operand))
        return false;

    // Swap bytes in the operand and write it to the operand location
    operand = ((operand & 0x00FF) << 8) | ((operand >> 8) & 0xFF);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = (u8) operand == 0,
        .V = false,
        .C = false});

    return singleOperandDecoder->writeOperand (operand.value ());
}

template <>
inline bool Executor::operator() (ADC& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    u16 cBit = commonExecutor.isSet (PSW_C) ? 1 : 0;
    u16 result = contents + cBit;

    commonExecutor.setPSW (ConditionCodes ({.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 0077777 && commonExecutor.isSet (PSW_C),
        .C = contents == 0177777 && commonExecutor.isSet (PSW_C)}));

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (ADCB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u16 tmp = commonExecutor.isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8) (source + tmp);

    commonExecutor.setPSW (ConditionCodes ({.N = (bool) (destination & 0x80),
        .Z = destination == 0,
        .V = source == 0177 && commonExecutor.isSet (PSW_C),
        .C = source == 0377 && commonExecutor.isSet (PSW_C)}));

    return singleOperandDecoder->writeOperand (destination);
}

template <>
inline bool Executor::operator() (SBC& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    u16 cBit = commonExecutor.isSet (PSW_C) ? 1 : 0;
    u16 result = contents - cBit;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 0100000,
        .C = contents == 0 && cBit});

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (SBCB& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u16 cBit = commonExecutor.isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8) (source - cBit);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (destination & 0x80),
        .Z = destination == 0,
        .V = source == 0200,
        .C = source == 0 && cBit});

    return singleOperandDecoder->writeOperand (destination);
}

template <>
inline bool Executor::operator() (SXT& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);

    u16 result = commonExecutor.isSet (PSW_N) ? 0177777 : 0;

    commonExecutor.setPSW (ConditionCodes {.Z = !commonExecutor.isSet (PSW_N),
        .V = false});

    return singleOperandDecoder->writeOperand (result);
}

template <>
inline bool Executor::operator() (MFPS& instr)
{
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);

    u8 contents = (u8) cpuData_->psw ();
    OperandLocation operandLocation = 
        singleOperandDecoder->getOperandLocation (cpuData_->registers ());

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (contents & 0x80),
        .Z = (contents & 0xFF) == 0,
        .V = false});

    if (operandLocation.isA<RegisterOperandLocation> ())
    {
        // If destination is mode 0 (Register), the regular operand processing
        // is bypassed and PS bit 7 is sign extended through the upper byte of
        // the register.
        cpuData_->registers ()[operandLocation] = (s8) contents;
    }
    else
    {
        if (!singleOperandDecoder->writeOperand (contents))
            return false;
    }
    
    return true;
}

// Double operand instructions
template <>
inline bool Executor::operator() (MOV& instr)
{
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source))
        return false;

    commonExecutor.setPSW (ConditionCodes ({.N = (bool) (source & 0100000),
        .Z = source == 0,
        .V = false}));
    
    return doubleOperandDecoder->writeDestinationOperand (source.value ());
}

template <>
inline bool Executor::operator() (MOVB& instr)
{
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u8> source;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source))
        return false;

    // Make the source a signed eight bit value 
    s8 tmp = (s8) source;

    commonExecutor.setPSW (ConditionCodes ({.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false}));

    // Using the overloaded MOVB::writeDestinationOperand()
    return doubleOperandDecoder->writeDestinationOperands8 (tmp);
}

template <>
inline bool Executor::operator() (CMP& instr)
{
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
        !doubleOperandDecoder->readDestinationOperand (&destination))
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
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u8> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source) || 
            !doubleOperandDecoder->readDestinationOperand (&destination))
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
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
            !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 result = source + destination;

    commonExecutor.setPSW (ConditionCodes ({.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = ((source & 0x8000) == (destination & 0x8000)) &&
            ((destination & 0x8000) != (result & 0x8000)),
        .C = (bool) (((u32) source + (u32) destination) & 0x10000)}));

    return doubleOperandDecoder->writeDestinationOperand (result);
}

template <>
inline bool Executor::operator() (SUB& instr)
{
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
            !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 result = destination - source;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = ((source & 0x8000) != (destination & 0x8000)) &&
             ((source & 0x8000) == (result & 0x8000)),
        .C = (bool) (((u32) destination - (u32) source) & 0x10000)});

    return doubleOperandDecoder->writeDestinationOperand (result);
}

template <>
inline bool Executor::operator() (BIT& instr)
{
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
            !doubleOperandDecoder->readDestinationOperand (&destination))
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
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u8> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source) || 
            !doubleOperandDecoder->readDestinationOperand (&destination))
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
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
            !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 result = ~source & destination;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = false});

    return doubleOperandDecoder->writeDestinationOperand (result);
}

template <>
inline bool Executor::operator() (BICB& instr)
{
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u8> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source) || 
            !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u8 tmp = (u8)(~source & destination);

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    return doubleOperandDecoder->writeDestinationOperand (tmp);
}

template <>
inline bool Executor::operator() (BIS& instr)
{
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
            !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 tmp = source | destination;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = false});

    return doubleOperandDecoder->writeDestinationOperand (tmp);
}

template <>
inline bool Executor::operator() (BISB& instr)
{
    auto doubleOperandDecoder = 
         operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u8> source, destination;

    // OPR%R, (R)+ or OPR%R, and -(R) using the same register as both
    // source and destination: contents of R are incremented (or decremented)
    // by 2 before being used as the source operand.
    doubleOperandDecoder->getOperandLocations (cpuData_->registers ());

    if (!doubleOperandDecoder->readSourceOperand (&source) || 
            !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u8 tmp = source | destination;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    return doubleOperandDecoder->writeDestinationOperand (tmp);
}

// EIS instructions, including JSR and XOR
template <>
inline bool Executor::operator() (XOR& instr)
{
    auto eisDecoder = 
         operandDecoderFactory_.create<EisDecoder> (&instr);

    u16 regNr = instr.getRegisterNr ();
    GeneralRegisters& registers = cpuData_->registers ();

    u16 source = registers[regNr];

    CondData<u16> destination;
    if (!eisDecoder->readOperand (&destination))
        return false;

    u16 result = source ^ destination;

    commonExecutor.setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = false});

    return eisDecoder->writeOperand (result);
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
    auto singleOperandDecoder = 
         operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    u16 tmp;
    
    // The destination operand and the value popped off the stack are 
    // retrieved in the current memory management and the tmp value then
    // is written using the previous mode.
    OperandLocation operandLocation =  
        singleOperandDecoder->getOperandLocation (cpuData_->registers ());

    if (!mmu_->popWord (&tmp) || !operandLocation.writePrevMode (tmp))
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

} // namespace KDF11

#endif // _KDF11_A_EXECUTOR_H_
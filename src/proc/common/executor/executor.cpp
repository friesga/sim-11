#include "executor.h"

Common::Executor::Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
    :
    cpuData_ {cpuData},
    cpuControl_ {cpuControl},
    mmu_ {mmu}
{
}

void Common::Executor::setPSW (ConditionCodes conditionCodes)
{
    cpuData_->setCC (conditionCodes);
}

// The execution of PDP-11 instructions consists broadly of four steps:
// fetching the operands, executing the action, writing the operands and
// setting the condition codes. The order of the last two steps differs
// between processor types. In a KD11-NA (LSI-11), for example, the operands
// are written first, followed by the condition codes, whereas in a KDF-11,
// for example, this is done in reverse order.
// 
// This function implements this behavior. The function takes two callable
// objects as arguments, one for writing the operands and one for setting the
// condition codes. The order of the calls is determined by the template
// parameter Order.
//
template <WriteOperandOrder Order, typename WriteOperandsFn, typename CCFn>
bool Common::Executor::finishExecution (WriteOperandsFn&& writeOperands,
    CCFn&& setConditionCodes)
{
    bool writeResult {};

    if constexpr (Order == WriteOperandOrder::WriteOperandBeforeCC)
    {
        writeResult = writeOperands ();
        if (writeResult)
            setConditionCodes ();
    }
    else
    {
        setConditionCodes ();
        writeResult = writeOperands ();
    }

    return writeResult;
}

// Single operand instructions
template <WriteOperandOrder order>
bool Common::Executor::execute (CLR& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand ((u16) 0); },
        [&] { setPSW (ConditionCodes {
            .N = false,
            .Z = true,
            .V = false,
            .C = false}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (CLRB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand ((u8) 0); },
        [&] { setPSW (ConditionCodes {
            .N = false,
            .Z = true,
            .V = false,
            .C = false}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (COM& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> operand;

    if (!singleOperandDecoder->readOperand (&operand))
        return false;

    // Complement the operand and write it to the operand location
    operand = ~operand;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (operand.value ()); },
        [&] { setPSW ({ConditionCodes {
            .N = (bool)(operand & 0x8000),
            .Z = operand == 0,
            .V = false,
            .C = true}}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (COMB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> operand;

    if (!singleOperandDecoder->readOperand (&operand))
        return false;

    // Complement the operand and write it to the operand location
    operand = ~operand;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (operand.value ()); },
        [&] { setPSW ({ConditionCodes {
            .N = (bool)(operand & 0x80),
            .Z = (u8)operand == 0,
            .V = false,
            .C = true}}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (INC& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    // Increment the operand and write it to the operand location
    u16 result = contents + 1;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool)(result & 0x8000),
            .Z = result == 0,
            .V = contents == 077777}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (INCB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u8 result = (u8)(source + 1);

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool)(result & 0x80),
            .Z = result == 0,
            .V = source == 000177}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (DEC& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    // Increment the operand and write it to the operand location
    u16 result = contents - 1;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool)(result & 0100000),
            .Z = result == 0,
            .V = contents == 0100000}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (DECB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u8 result = (u8) (source - 1);

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (result & 0x80),
            .Z = result == 0,
            .V = source == 0000200}); });

    return true;
}

template <WriteOperandOrder order>
bool Common::Executor::execute (NEG& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> operand;

    if (!singleOperandDecoder->readOperand (&operand))
        return false;

    // Negate the operand and write it to the operand location
    if (operand != 0100000)
        operand = -operand;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (operand.value ()); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (operand & 0100000),
            .Z = operand == 0,
            .V = operand == 0100000,
            .C = operand != 0}); });

    return true;
}

template <WriteOperandOrder order>
bool Common::Executor::execute (NEGB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> operand;

    if (!singleOperandDecoder->readOperand (&operand))
        return false;

    if (operand != 0200)
        operand = -operand;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (operand.value ()); },
        [&] { setPSW (ConditionCodes {
            .N = (bool)(operand & 0x80),
            .Z = operand == 0,
            .V = operand == 0200,
            .C = operand != 0}); });
}

bool Common::Executor::execute (TST& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    setPSW (ConditionCodes {
        .N = (bool) (contents & 0100000),
        .Z = contents == 0,
        .V = false,
        .C = false});

    return true;
}

bool Common::Executor::execute (TSTB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    setPSW (ConditionCodes {
        .N = (bool) (source & 0x80),
        .Z = source == 0,
        .V = false,
        .C = false});

    return true;
}

template <WriteOperandOrder order>
bool Common::Executor::execute (ASR& instr)
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

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (result & 0100000),
            .Z = result == 0,
            .V = (bool) (result & 0100000) != (bool)(contents & 1),
            .C = (bool) (contents & 1)}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (ASRB& instr)
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

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (result & 0x80),
            .Z = result == 0,
            .V = (bool) (result & 0x80) != (bool)(source & 1),
            .C = (bool) (source & 1)}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (ASL& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    u16 result = contents << 1;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (result & 0100000),
            .Z = result == 0,
            .V = (bool) ((result & 0100000) ^ (contents & 0100000)),
            .C = (bool) (contents & 0100000)}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (ASLB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u8 result = (u8)(source << 1);

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (result & 0x80),
            .Z = result == 0,
            .V = (bool) ((result & 0x80) ^ (source & 0x80)),
            .C = (bool) (source & 0x80)}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (ROR& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    u16 result = contents >> 1;
    if (isSet (PSW_C))
        result |= 0100000;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (result & 0100000),
            .Z = result == 0,
            .V = (bool) (result & 0100000) != (bool)(contents & 0000001),
            .C = (bool) (contents & 0000001)}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (RORB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u8 result = source >> 1;
    if (isSet (PSW_C))
        result |= 0x80;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (result & 0x80),
            .Z = result == 0,
            .V = (bool) (result & 0x80) != (bool)(source & 0x01),
            .C = (bool) (source & 0x01)}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (ROL& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    u16 cBit = isSet (PSW_C);
    u16 result = contents << 1;
    if (cBit)
        result |= 01;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (result & 0100000),
            .Z = result == 0,
            .V = (bool) (result & 0100000) != (bool)(contents & 0100000),
            .C = (bool) (contents & 0100000)}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (ROLB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u8 result = (u8) (source << 1);
    if (isSet (PSW_C))
        result |= 0x01;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (result & 0x80),
            .Z = result == 0,
            .V = (bool) (result & 0x80) != (bool) (source & 0x80),
            .C = (bool) (source & 0x80)}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (SWAB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> operand;

    if (!singleOperandDecoder->readOperand (&operand))
        return false;

    // Swap bytes in the operand and write it to the operand location
    operand = ((operand & 0x00FF) << 8) | ((operand >> 8) & 0xFF);

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (operand.value ()); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (operand & 0x80),
            .Z = (u8) operand == 0,
            .V = false,
            .C = false}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (ADC& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u16 result = contents + cBit;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes ({
            .N = (bool) (result & 0x8000),
            .Z = result == 0,
            .V = contents == 0077777 && isSet (PSW_C),
            .C = contents == 0177777 && isSet (PSW_C)})); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (ADCB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u16 tmp = isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8)(source + tmp);

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (destination); },
        [&] { setPSW (ConditionCodes ({
            .N = (bool) (destination & 0x80),
            .Z = destination == 0,
            .V = source == 0177 && isSet (PSW_C),
            .C = source == 0377 && isSet (PSW_C)})); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (SBC& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u16> contents;

    if (!singleOperandDecoder->readOperand (&contents))
        return false;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u16 result = contents - cBit;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (result & 0x8000),
            .Z = result == 0,
            .V = contents == 0100000,
            .C = contents == 0 && cBit}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (SBCB& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!singleOperandDecoder->readOperand (&source))
        return false;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8)(source - cBit);

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (destination); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (destination & 0x80),
            .Z = destination == 0,
            .V = source == 0200,
            .C = source == 0 && cBit}); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (SXT& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    u16 result = isSet (PSW_N) ? 0177777 : 0;

    return finishExecution<order> (
        [&] { return singleOperandDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .Z = !isSet (PSW_N),
            .V = false}); });

}

template <WriteOperandOrder order>
bool Common::Executor::execute (MFPS& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    u8 contents = (u8) cpuData_->psw ();

    OperandLocation operandLocation_ =
        singleOperandDecoder->getOperandLocation (cpuData_->registers ());

    return finishExecution<order> (
        [&] {
            if (operandLocation_.isA<RegisterOperandLocation> ())
            {
                // If destination is mode 0 (Register), the regular operand processing
                // is bypassed and PS bit 7 is sign extended through the upper byte of
                // the register.
                cpuData_->registers ()[operandLocation_] = (s8) cpuData_->psw ();
                return true;
            }
            else
            {
                return singleOperandDecoder->writeOperand (contents);
            }
        },
        [&] {
            setPSW (ConditionCodes {
                .N = (bool)(contents & 0x80),
                .Z = (contents & 0xFF) == 0,
                .V = false});
        });
}

bool Common::Executor::execute (MTPS& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);
    CondData<u8> newValue;

    if (!singleOperandDecoder->readOperand (&newValue))
        return false;

    // Allow bits 5/6/7 to be set and cleared
    cpuData_->psw ().set (PSW::ProtectionMode::MTPS, newValue);

    return true;
}

bool Common::Executor::execute (JMP& instr)
{
    auto singleOperandDecoder =
        operandDecoderFactory_.create<SingleOperandDecoder> (&instr);

    OperandLocation operandLocation_ =
        singleOperandDecoder->getOperandLocation (cpuData_->registers ());

    if (operandLocation_.isA<MemoryOperandLocation> ())
    {
        cpuData_->registers ()[7] = operandLocation_;
        return true;
    }

    // Illegal instruction
    cpuData_->setTrap (CpuData::TrapType::BusError);
    return false;
}

bool Common::Executor::execute (MARK& instr)
{
    GeneralRegisters& registers = cpuData_->registers ();

    registers[6] = registers[7] + 2 * instr.numberOfParameters ();
    registers[7] = registers[5];
    registers[5] = mmu_->fetchWord (registers[6]);
    registers[6] += 2;

    return true;
}

// Double operand instructions
template <WriteOperandOrder order>
bool Common::Executor::execute (MOV& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    if (!doubleOperandDecoder->readSourceOperand (&source))
        return false;

    return finishExecution<order> (
        [&] { return doubleOperandDecoder->writeDestinationOperand (source.value ()); },
        [&] { setPSW (ConditionCodes ({
                .N = (bool) (source & 0100000),
                .Z = source == 0,
                .V = false})); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (MOVB& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u8> source;

    if (!doubleOperandDecoder->readSourceOperand (&source))
        return false;

    s8 tmp = (s8) source;

    // If the destination mode is 0 (Register) the regular operand processing
    // is bypassed and the signed eight bit value u8 is directly written into
    // the register, causing sign extension in the register.
    OperandLocation destinationOperandLocation_ =
        doubleOperandDecoder->getDestinationOperandLocation (cpuData_->registers ());

    return finishExecution<order> (
        [&] {
            if (destinationOperandLocation_.isA<RegisterOperandLocation> ())
            {
                cpuData_->registers ()[destinationOperandLocation_] = tmp;
                return true;
            }
            else
                return destinationOperandLocation_.write<u8> (tmp);
        },
        [&] {
            setPSW (ConditionCodes ({
                .N = (bool) (tmp & 0x80),
                .Z = tmp == 0,
                .V = false}));
        });
}

bool Common::Executor::execute (CMP& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
        !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 tmp = source - destination;

    setPSW (ConditionCodes {
        .N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = ((source & 0x8000) != (destination & 0x8000)) && ((destination & 0x8000) == (tmp & 0x8000)),
        .C = (bool) (((u32) source - (u32) destination) & 0x10000)});

    return true;
}

bool Common::Executor::execute (CMPB& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u8> source, destination;

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
        !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 tmp = (u8) (source - destination);

    setPSW (ConditionCodes {
        .N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = ((source & 0x80) != (destination & 0x80)) && ((destination & 0x80) == (tmp & 0x80)),
        .C = (bool) ((source - destination) & 0x100)});

    return true;
}

template <WriteOperandOrder order>
bool Common::Executor::execute (ADD& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
        !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 result = source + destination;

    return finishExecution<order> (
        [&] { return doubleOperandDecoder->writeDestinationOperand (result); },
        [&] { setPSW (ConditionCodes ({
            .N = (bool) (result & 0x8000),
            .Z = result == 0,
            .V = ((source & 0x8000) == (destination & 0x8000)) && ((destination & 0x8000) != (result & 0x8000)),
            .C = (bool) (((u32) source + (u32) destination) & 0x10000)})); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (SUB& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
        !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 result = destination - source;

    return finishExecution<order> (
        [&] { return doubleOperandDecoder->writeDestinationOperand (result); },
        [&] { setPSW (ConditionCodes ({
            .N = (bool) (result & 0x8000),
            .Z = result == 0,
            .V = ((source & 0x8000) != (destination & 0x8000)) && ((destination & 0x8000) == (result & 0x8000)),
            .C = (bool) (((u32) destination - (u32) source) & 0x10000)})); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (BIT& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
        !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 tmp = source & destination;

    setPSW (ConditionCodes {
        .N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = false});

    return true;
}

template <WriteOperandOrder order>
bool Common::Executor::execute (BITB& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u8> source, destination;

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
        !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 tmp = source & destination;

    setPSW (ConditionCodes {
        .N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    return true;
}

template <WriteOperandOrder order>
bool Common::Executor::execute (BIC& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
        !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 result = ~source & destination;

    return finishExecution<order> (
        [&] { return doubleOperandDecoder->writeDestinationOperand (result); },
        [&] { setPSW (ConditionCodes ({
            .N = (bool) (result & 0x8000),
            .Z = result == 0,
            .V = false})); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (BICB& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u8> source, destination;

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
        !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u8 tmp = (u8)(~source & destination);

    return finishExecution<order> (
        [&] { return doubleOperandDecoder->writeDestinationOperand (tmp); },
        [&] { setPSW (ConditionCodes ({
            .N = (bool) (tmp & 0x80),
            .Z = tmp == 0,
            .V = false})); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (BIS& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u16> source, destination;

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
        !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u16 tmp = source | destination;

    return finishExecution<order> (
        [&] { return doubleOperandDecoder->writeDestinationOperand (tmp); },
        [&] { setPSW (ConditionCodes ({
            .N = (bool) (tmp & 0x8000),
            .Z = tmp == 0,
            .V = false})); });
}

template <WriteOperandOrder order>
bool Common::Executor::execute (BISB& instr)
{
    auto doubleOperandDecoder =
        operandDecoderFactory_.create<DoubleOperandDecoder> (&instr);
    CondData<u8> source, destination;

    if (!doubleOperandDecoder->readSourceOperand (&source) ||
        !doubleOperandDecoder->readDestinationOperand (&destination))
        return false;

    u8 tmp = source | destination;

    return finishExecution<order> (
        [&] { return doubleOperandDecoder->writeDestinationOperand (tmp); },
        [&] { setPSW (ConditionCodes ({
            .N = (bool) (tmp & 0x80),
            .Z = tmp == 0,
            .V = false})); });
}

// EIS instructions, including JSR and XOR
bool Common::Executor::execute (JSR& instr)
{
    auto eisDecoder =
        operandDecoderFactory_.create<EisDecoder> (&instr);

    OperandLocation destination =
        eisDecoder->getOperandLocation (cpuData_->registers ());

    if (!destination.isA<MemoryOperandLocation> ())
    {
        // Illegal instruction
        cpuData_->setTrap (CpuData::TrapType::IllegalInstructionTrap);
        return true;
    }

    GeneralRegisters& registers = cpuData_->registers ();
    u16 specifiedRegisterContents = registers[instr.getRegisterNr ()];

    if (!mmu_->pushWord (specifiedRegisterContents))
        return false;

    if (cpuData_->stackOverflow ())
        cpuData_->setTrap (CpuData::TrapType::StackOverflow);

    registers[instr.getRegisterNr ()] = registers[7];
    registers[7] = destination;

    return true;
}

bool Common::Executor::execute (MUL& instr)
{
    auto eisDecoder =
        operandDecoderFactory_.create<EisDecoder> (&instr);

    u16 regNr = instr.getRegisterNr ();

    GeneralRegisters& registers = cpuData_->registers ();
    u16 destination = registers[regNr];

    CondData<u16> source;
    if (!eisDecoder->readOperand (&source))
        return false;

    s32 tmps32 = (s32)(s16)destination * (s16)source;
    registers[regNr] = (u16)(tmps32 >> 16);
    registers[regNr | 1] = (u16)tmps32;

    setPSW (ConditionCodes {.N = tmps32 < 0,
        .Z = tmps32 == 0,
        .V = false,
        .C = (tmps32 >= 0x7FFF) || (tmps32 < -0x8000)});

    return true;
}

bool Common::Executor::execute (DIV& instr)
{
    auto eisDecoder =
        operandDecoderFactory_.create<EisDecoder> (&instr);
    u16 regNr = instr.getRegisterNr ();

    GeneralRegisters& registers = cpuData_->registers ();
    s32 tmps32 = (registers[regNr] << 16) | registers[regNr | 1];

    CondData<u16> source;
    if (!eisDecoder->readOperand (&source))
        return false;

    if (source == 0)
    {
        setPSW (ConditionCodes {.V = true, .C = true});
    }
    else
    {
        s32 quotient = tmps32 / (s16)source;
        s32 remainder = tmps32 % (s16)source;

        if ((s16)quotient != quotient)
        {
            setPSW (ConditionCodes {.V = true, .C = false});
        }
        else
        {
            registers[regNr] = (u16)quotient;
            registers[regNr | 1] = (u16)remainder;
            setPSW (ConditionCodes {.N = quotient < 0,
                .Z = quotient == 0,
                .C = false});
        }
    }

    return true;
}

bool Common::Executor::execute (ASH& instr)
{
    auto eisDecoder =
        operandDecoderFactory_.create<EisDecoder> (&instr);

    u16 regNr = instr.getRegisterNr ();
    u16 tmp {0};
    GeneralRegisters& registers = cpuData_->registers ();
    u16 destination = registers[regNr];

    CondData<u16> source;
    if (!eisDecoder->readOperand (&source))
        return false;

    if (source & 0x20)
    {
        // Negative - shift right
        s16 stmp, stmp2;
        source = (~source & 0x1F) + 1;
        stmp = (s16)destination;
        stmp2 = stmp >> (source - 1);
        stmp >>= source;
        tmp = (u16)stmp;
        setPSW (ConditionCodes {.V = false, .C = (bool) (stmp2 & 1)});
    }
    else if ((source & 0x1F) == 0)
    {
        // Zero - don't shift
        setPSW (ConditionCodes {.V = false, .C = false});
        tmp = destination;
    }
    else
    {
        // Positive - shift left
        s16 mask = 0;
        // Note that the bitwise and/assignment operator can't be used on
        // CondData objects.
        source = source & 0x1F;
        tmp = destination << source;
        if (source > 0)
        {
            mask = 0x8000;
            mask >>= source;
            u16 tmp2 = destination & mask;
            setPSW (ConditionCodes {.V = !((tmp2 == 0) || (((tmp2 & mask) | ~mask) == 0xFFFF))});
        }
        else
        {
            setPSW (ConditionCodes {.V = false});
        }
        setPSW (ConditionCodes {.C = (bool) ((destination << (source - 1)) & 0x8000)});
        if ((destination & 0x8000) != (tmp & 0x8000))
        {
            setPSW (ConditionCodes {.V = true});
        }
    }
    registers[regNr] = tmp;
    setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000), .Z = tmp == 0});

    return true;
}

bool Common::Executor::execute (ASHC& instr)
{
    auto eisDecoder =
        operandDecoderFactory_.create<EisDecoder> (&instr);

    u16 regNr = instr.getRegisterNr ();
    GeneralRegisters& registers = cpuData_->registers ();
    u16 dst = registers[regNr];

    s32 tmps32 = (registers[regNr] << 16) | registers[regNr | 1];

    CondData<u16> source;
    if (!eisDecoder->readOperand (&source))
        return false;

    if ((source & 0x3F) == 0x20)
    {
        // Negative; 32 right
        setPSW (ConditionCodes {.V = false, .C = (bool) (tmps32 & 0x80000000)});
        if (isSet (PSW_C))
            tmps32 = 0xFFFFFFFF;
        else
            tmps32 = 0;
    }
    else if (source & 0x20)
    {
        // Negative - shift right
        s32 stmp2;
        source = (~source & 0x1F) + 1;
        stmp2 = tmps32 >> (source - 1);
        tmps32 >>= source;
        setPSW (ConditionCodes {.C = (bool) (stmp2 & 1)});
    }
    else if ((source & 0x1F) == 0)
    {
        // Zero - don't shift
        setPSW (ConditionCodes {.V = false, .C = false});
    }
    else
    {
        // Positive - shift left
        s32 stmp2;
        source = source & 0x1F;
        stmp2 = tmps32 << (source - 1);
        tmps32 <<= source;
        setPSW (ConditionCodes {.V = !!(dst & 0x8000) != !!(tmps32 & 0x80000000),
            .C =(bool) (stmp2 & 0x80000000)});
    }
    registers[regNr] = (u16)(tmps32 >> 16);
    registers[regNr | 1] = (u16)tmps32;
    setPSW (ConditionCodes {.N = (bool) (tmps32 & 0x80000000),
        .Z = tmps32 == 0});

    return true;
}

template <WriteOperandOrder order>
bool Common::Executor::execute (XOR& instr)
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

    return finishExecution<order> (
        [&] { return eisDecoder->writeOperand (result); },
        [&] { setPSW (ConditionCodes {
            .N = (bool) (result & 0x8000),
            .Z = result == 0,
            .V = false}); });
}

// SOB instruction format
bool Common::Executor::execute (SOB& instr)
{
    u16 regNr = instr.getRegisterNr ();
    u16 offset = instr.getOffset ();

    GeneralRegisters& reg = cpuData_->registers ();
    reg[regNr]--;
    if (reg[regNr])
        reg[7] -= 2 * offset;

    return true;
}

// FIS format instructions
bool Common::Executor::execute (RTS& instr)
{
    u16 regNr = instr.getRegister ();

    cpuData_->registers ()[7] = cpuData_->registers ()[regNr];
    mmu_->popWord (&cpuData_->registers ()[regNr]);

    return true;
}

// Branch instructions
bool Common::Executor::execute (BR& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranch ();
    return true;
}

bool Common::Executor::execute (BNE& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (!isSet (PSW_Z));
    return true;
}

bool Common::Executor::execute (BEQ& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (isSet (PSW_Z));
    return true;
}

bool Common::Executor::execute (BPL& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (!isSet (PSW_N));
    return true;
}

bool Common::Executor::execute (BMI& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (isSet (PSW_N));
    return true;
}

bool Common::Executor::execute (BVC& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (!isSet (PSW_V));
    return true;
}

bool Common::Executor::execute (BVS& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (isSet (PSW_V));
    return true;
}

bool Common::Executor::execute (BCC& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (!isSet (PSW_C));
    return true;
}

bool Common::Executor::execute (BCS& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (isSet (PSW_C));
    return true;
}

bool Common::Executor::execute (BGE& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf ((isSet (PSW_N) ^ isSet (PSW_V)) == 0);
    return true;
}

bool Common::Executor::execute (BLT& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (isSet (PSW_N) ^ isSet (PSW_V));
    return true;
}

bool Common::Executor::execute (BGT& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf ((isSet (PSW_Z) || (isSet (PSW_N) ^ isSet (PSW_V))) == 0);
    return true;
}

bool Common::Executor::execute (BLE& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (isSet (PSW_Z) || (isSet (PSW_N) ^ isSet (PSW_V)));
    return true;
}

bool Common::Executor::execute (BHI& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (!isSet (PSW_C) && !isSet (PSW_Z));
    return true;
}

bool Common::Executor::execute (BLOS& instr)
{
    auto branchDecoder =
        operandDecoderFactory_.create<BranchDecoder> (&instr);

    branchDecoder->executeBranchIf (isSet (PSW_C) || isSet (PSW_Z));
    return true;
}

bool Common::Executor::execute (EMT& instr)
{
    cpuData_->setTrap (CpuData::TrapType::EmulatorTrap);
    return true;
}

bool Common::Executor::execute (TRAP& instr)
{
    cpuData_->setTrap (CpuData::TrapType::TrapInstruction);
    return true;
}

// No-operand instructions
bool Common::Executor::execute (HALT& instr)
{
    cpuControl_->halt ();
    return true;
}

bool Common::Executor::execute (WAIT& instr)
{
    cpuControl_->wait ();
    return true;
}

// If an RTI [instruction] sets the T bit in the PS, a trace trap will occur
// before the next instruction is executed. (Micro PDP-11 Handbook, pag 322.)
// This behaviour differs from the other instructionss in which setting the
// T-bit results in a trace trap on the next instruction.
bool Common::Executor::execute (RTI& instr)
{
    u16 tmp;

    if (!mmu_->popWord (&cpuData_->registers ()[7]) || !mmu_->popWord (&tmp))
        return false;

    cpuData_->psw ().set (PSW::ProtectionMode::RTI, tmp);
    if (cpuData_->psw ().traceBitSet ())
        cpuData_->setTrap (CpuData::TrapType::BreakpointTrap);

    return true;
}

bool Common::Executor::execute (BPT& instr)
{
    cpuData_->setTrap (CpuData::TrapType::BreakpointTrap);
    return true;
}

bool Common::Executor::execute (IOT& instr)
{
    cpuData_->setTrap (CpuData::TrapType::InputOutputTrap);
    return true;
}

bool Common::Executor::execute (RESET& instr)
{
    cpuControl_->busReset ();
    return true;
}

//  If the RTT sets the T bit in the PS, the next instruction will be executed
// and then the trace trap will be processed. (Micro PDP-11 Handbook,
// pag 322). This is the normal behaviour for instruction setting the T-bit.
bool Common::Executor::execute (RTT& instr)
{
    u16 tmp;

    if (!mmu_->popWord (&cpuData_->registers ()[7]) || !mmu_->popWord (&tmp))
        return false;

    cpuData_->psw ().set (PSW::ProtectionMode::RTI, tmp);

    return true;
}

// Condition Code instructions
bool Common::Executor::execute (CCC& instr)
{
    cpuData_->psw ().set (PSW::ProtectionMode::ExplicitAccess,
        cpuData_->psw () & ~instr.getConditionCodes ());
    return true;
}

bool Common::Executor::execute (SCC& instr)
{
    cpuData_->psw ().set (PSW::ProtectionMode::ExplicitAccess,
        cpuData_->psw () | instr.getConditionCodes ());
    return true;
}

bool Common::Executor::execute (Unused& instr)
{
    cpuData_->setTrap (CpuData::TrapType::ReservedInstructionTrap);
    return true;
}

// Explicit template instantiation to be able to define the methods in
// a separate .cpp file.
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (CLR&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (CLR&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (CLRB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (CLRB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (COM&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (COM&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (COMB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (COMB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (INC&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (INC&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (INCB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (INCB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (DEC&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (DEC&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (DECB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (DECB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (NEG&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (NEG&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (NEGB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (NEGB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (ASR&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (ASR&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (ASRB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (ASRB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (ASL&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (ASL&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (ASLB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (ASLB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (ROR&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (ROR&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (RORB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (RORB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (ROL&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (ROL&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (ROLB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (ROLB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (SWAB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (SWAB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (ADC&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (ADC&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (ADCB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (ADCB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (SBC&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (SBC&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (SBCB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (SBCB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (SXT&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (SXT&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (MFPS&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (MFPS&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (MOV&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (MOV&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (MOVB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (MOVB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (ADD&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (ADD&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (SUB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (SUB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (XOR&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (XOR&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (BIT&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (BIT&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (BITB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (BITB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (BIC&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (BIC&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (BICB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (BICB&);

template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (BIS&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (BIS&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandBeforeCC> (BISB&);
template bool Common::Executor::execute<WriteOperandOrder::WriteOperandAfterCC> (BISB&);

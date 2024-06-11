#include "executor.h"

Common::Executor::Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
    :
    cpuData_ {cpuData},
    cpuControl_ {cpuControl},
    mmu_ {mmu}
{}

 void Common::Executor::setPSW (ConditionCodes conditionCodes)
{
	cpuData_->setCC (conditionCodes);
}

constexpr bool Common::Executor::isSet (u16 x)
{
	return (cpuData_->psw () & x) ? true : false;
}

 // Single operand instructions
bool Common::Executor::operator() (CLR& instr)
{
    if (!instr.writeOperand ((u16)0))
        return false;

    setPSW (ConditionCodes {.N = false,
        .Z = true,
        .V = false,
        .C = false});

    return true;
}

bool Common::Executor::operator() (CLRB& instr)
{
    if (!instr.writeOperand ((u8) 0))
        return false;

    setPSW (ConditionCodes {.N = false,
        .Z = true,
        .V = false,
        .C = false});

    return true;
}

bool Common::Executor::operator() (COM& instr)
{
    CondData<u16> operand;
    if (!instr.readOperand (&operand))
        return false;

    // Complement the operand and write it to the operand location
    operand = ~operand;
    if (!instr.writeOperand (operand.value ()))
        return false;

    setPSW ({ConditionCodes {.N = (bool) (operand & 0x8000),
        .Z = operand == 0,
        .V = false,
        .C = true}});

    return true;
}

bool Common::Executor::operator() (COMB& instr)
{
    CondData<u8> operand;
    if (!instr.readOperand (&operand))
        return false;

    // Complement the operand and write it to the operand location
    operand = ~operand;
    if (!instr.writeOperand (operand.value ()))
        return false;

    setPSW ({ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = (u8) operand == 0,
        .V = false,
        .C = true}});

    return true;
}

bool Common::Executor::operator() (INC& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    // Increment the operand and write it to the operand location
    u16 result = contents + 1;
    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 077777});

    return true;
}

bool Common::Executor::operator() (INCB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u8 result = (u8)(source + 1);

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = source == 000177});

    return true;
}

bool Common::Executor::operator() (DEC& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    // Increment the operand and write it to the operand location
    u16 result = contents - 1;
    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = contents == 0100000});

    return true;
}

bool Common::Executor::operator() (DECB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u8 result = (u8) (source - 1);

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = source == 0000200});

    return true;
}

bool Common::Executor::operator() (NEG& instr)
{
    CondData<u16> operand;
    if (!instr.readOperand (&operand))
        return false;

    // Negate the operand and write it to the operand location
    if (operand != 0100000)
        operand = -operand;

    if (!instr.writeOperand (operand.value ()))
        return false;

    setPSW (ConditionCodes {.N = (bool) (operand & 0100000),
        .Z = operand == 0,
        .V = operand == 0100000,
        .C = operand != 0});

    return true;
}
bool Common::Executor::operator() (NEGB& instr)
{
    CondData<u8> operand;
    if (!instr.readOperand (&operand))
        return false;

    if (operand != 0200)
        operand = -operand;

    if (!instr.writeOperand (operand.value ()))
        return false;

    setPSW (ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = operand == 0,
        .V = operand == 0200,
        .C = operand != 0});

    return true;
}

bool Common::Executor::operator() (TST& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    setPSW (ConditionCodes {.N = (bool) (contents & 0100000),
        .Z = contents == 0,
        .V = false,
        .C = false});

    return true;
}

bool Common::Executor::operator() (TSTB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    setPSW (ConditionCodes {.N = (bool) (source & 0x80),
        .Z = source == 0,
        .V = false,
        .C = false});

    return true;
}

bool Common::Executor::operator() (ASR& instr)
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

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = (bool) (result & 0100000) != (bool) (contents & 1),
        .C = (bool) (contents & 1)});

    return true;
}

bool Common::Executor::operator() (ASRB& instr)
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

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) (result & 0x80) != (bool) (source & 1),
        .C = (bool) (source & 1)});

    return true;
}

bool Common::Executor::operator() (ASL& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    u16 result = contents << 1;

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = (bool) ((result & 0100000) ^ (contents & 0100000)),
        .C = (bool) (contents & 0100000)});

    return true;
}

bool Common::Executor::operator() (ASLB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u8 result = (u8)(source << 1);

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) ((result & 0x80) ^ (source & 0x80)),
        .C = (bool) (source & 0x80)});

    return true;
}

bool Common::Executor::operator() (ROR& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    u16 result = contents >> 1;
    if (isSet (PSW_C))
        result |= 0100000;

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = (bool) (result & 0100000) != (bool) (contents & 0000001),
        .C = (bool) (contents & 0000001)});

    return true;
}

bool Common::Executor::operator() (RORB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u8 result = source >> 1;
    if (isSet (PSW_C))
        result |= 0x80;

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) (result & 0x80) != (bool) (source & 0x01),
        .C = (bool) (source & 0x01)});

    return true;
}

bool Common::Executor::operator() (ROL& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    u16 cBit = isSet (PSW_C);
    u16 result = contents << 1;
    if (cBit)
        result |= 01;

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = (bool) (result & 0100000) != (bool) (contents & 0100000),
        .C = (bool) (contents & 0100000)});

    return true;
}

bool Common::Executor::operator() (ROLB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u8 result = (u8) (source << 1);
    if (isSet (PSW_C))
        result |= 0x01;

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = (bool) (result & 0x80) != (bool) (source & 0x80),
        .C = (bool) (source & 0x80)});

    return true;
}

bool Common::Executor::operator() (SWAB& instr)
{
    CondData<u16> operand;
    if (!instr.readOperand (&operand))
        return false;

    // Swap bytes in the operand and write it to the operand location
    operand = ((operand & 0x00FF) << 8) | ((operand >> 8) & 0xFF);

    if (!instr.writeOperand (operand.value ()))
        return false;

    setPSW (ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = (u8) operand == 0,
        .V = false,
        .C = false});

    return true;
}

bool Common::Executor::operator() (ADC& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u16 result = contents + cBit;

    if (!instr.writeOperand (result))
        return false;

     setPSW (ConditionCodes ({.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 0077777 && isSet (PSW_C),
        .C = contents == 0177777 && isSet (PSW_C)}));

    return true;
}

bool Common::Executor::operator() (ADCB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u16 tmp = isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8)(source + tmp);

    if (!instr.writeOperand (destination))
        return false;

    setPSW (ConditionCodes ({.N = (bool) (destination & 0x80),
        .Z = destination == 0,
        .V = source == 0177 && isSet (PSW_C),
        .C = source == 0377 && isSet (PSW_C)}));

    return true;
}

bool Common::Executor::operator() (SBC& instr)
{
    CondData<u16> contents;
    if (!instr.readOperand (&contents))
        return false;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u16 result = contents - cBit;

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 0100000,
        .C = contents == 0 && cBit});

    return true;
}

bool Common::Executor::operator() (SBCB& instr)
{
    CondData<u8> source;
    if (!instr.readOperand (&source))
        return false;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8) (source - cBit);

    if (!instr.writeOperand (destination))
        return false;

    setPSW (ConditionCodes {.N = (bool) (destination & 0x80),
        .Z = destination == 0,
        .V = source == 0200,
        .C = source == 0 && cBit});

    return true;
}

bool Common::Executor::operator() (SXT& instr)
{
    u16 result = isSet (PSW_N) ? 0177777 : 0;

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.Z = !isSet (PSW_N),
        .V = false});

    return true;
}

bool Common::Executor::operator() (MFPS& instr)
{
    u8 contents = (u8) cpuData_->psw ();
    OperandLocation operandLocation_ = 
        instr.getOperandLocation (cpuData_->registers ());

    if (operandLocation_.isA<RegisterOperandLocation> ())
    {
        // If destination is mode 0 (Register), the regular operand processing
        // is bypassed and PS bit 7 is sign extended through the upper byte of
        // the register.
        cpuData_->registers ()[operandLocation_] = (s8)cpuData_->psw ();
    }
    else
    {
        if (!instr.writeOperand (contents))
            return false;
    }
    
    setPSW (ConditionCodes {.N = (bool) (contents & 0x80),
        .Z = (contents & 0xFF) == 0,
        .V = false});

    return true;
}

bool Common::Executor::operator() (MTPS& instr)
{
    CondData<u8> newValue;
    if (!instr.readOperand (&newValue))
        return false;

    // Allow bits 5/6/7 to be set and cleared
    cpuData_->psw ().set (PSW::ProtectionMode::MTPS, newValue);

    return true;
}

bool Common::Executor::operator() (JMP& instr)
{
    OperandLocation operandLocation_ = 
        instr.getOperandLocation (cpuData_->registers ());
    if (operandLocation_.isA<MemoryOperandLocation> ())
    {
        cpuData_->registers ()[7] = operandLocation_;
        return true;
    }

    // Illegal instruction
    cpuData_->setTrap (CpuData::TrapCondition::BusError);
    return false;
}

bool Common::Executor::operator() (MARK& instr)
{
    GeneralRegisters& registers = cpuData_->registers ();

    registers[6] = registers[7] + 2 * instr.numberOfParameters ();
    registers[7] = registers[5];
    registers[5] = mmu_->fetchWord (registers[6]);
    registers[6] += 2;

    return true;
}

	// Double operand instructions
bool Common::Executor::operator() (MOV& instr)
{
    CondData<u16> source, destination;

    if (!instr.readSourceOperand (&source) ||
        !instr.writeDestinationOperand (source.value ()))
        return false;

    setPSW (ConditionCodes ({.N = (bool) (source & 0100000),
        .Z = source == 0,
        .V = false}));

    return true;
}

bool Common::Executor::operator() (MOVB& instr)
{
    CondData<u8> source;

    if (!instr.readSourceOperand (&source))
        return false;

    s8 tmp = (s8) source;

    // If the destination mode is 0 (Register) the regular operand processing
    // is bypassed and the signed eight bit value u8 is directly written into
    // the register, causing sign extension in the register.
    OperandLocation destinationOperandLocation_ = 
        instr.getDestinationOperandLocation (cpuData_->registers ());

    if (destinationOperandLocation_.isA<RegisterOperandLocation> ())
        cpuData_->registers ()[destinationOperandLocation_] = tmp;
    else
        if (!destinationOperandLocation_.write<u8> (tmp))
            return false;

    setPSW (ConditionCodes ({.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false}));

    return true;
}

bool Common::Executor::operator() (CMP& instr)
{
    CondData<u16> source, destination;

    if (!instr.readSourceOperand (&source) ||
        !instr.readDestinationOperand (&destination))
        return false;

    u16 tmp = source - destination;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = ((source & 0x8000) != (destination & 0x8000)) && ((destination & 0x8000) == (tmp & 0x8000)),
        .C = (bool) (((u32) source - (u32) destination) & 0x10000)});

    return true;
}

bool Common::Executor::operator() (CMPB& instr)
{
    CondData<u8> source, destination;

    if (!instr.readSourceOperand (&source) || 
            !instr.readDestinationOperand (&destination))
        return false;

    u16 tmp = (u8) (source - destination);

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = ((source & 0x80) != (destination & 0x80)) && ((destination & 0x80) == (tmp & 0x80)),
        .C = (bool) ((source - destination) & 0x100)});

    return true;
}

bool Common::Executor::operator() (ADD& instr)
{
    CondData<u16> source, destination;

    if (!instr.readSourceOperand (&source) ||
        !instr.readDestinationOperand (&destination))
        return false;

    u16 result = source + destination;

    if (!instr.writeDestinationOperand (result))
        return false;

    setPSW (ConditionCodes ({.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = ((source & 0x8000) == (destination & 0x8000)) && ((destination & 0x8000) != (result & 0x8000)),
        .C = (bool) (((u32) source + (u32) destination) & 0x10000)}));

    return true;
}

bool Common::Executor::operator() (SUB& instr)
{
    CondData<u16> source, destination;

    if (!instr.readSourceOperand (&source) ||
        !instr.readDestinationOperand (&destination))
        return false;

    u16 result = destination - source;

    if (!instr.writeDestinationOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = ((source & 0x8000) != (destination & 0x8000)) &&
             ((source & 0x8000) == (result & 0x8000)),
        .C = (bool) (((u32) destination - (u32) source) & 0x10000)});

    return true;
}

bool Common::Executor::operator() (BIT& instr)
{
    CondData<u16> source, destination;

    if (!instr.readSourceOperand (&source) ||
        !instr.readDestinationOperand (&destination))
        return false;

    u16 tmp = source & destination;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = false});

    return true;
}

bool Common::Executor::operator() (BITB& instr)
{
    CondData<u8> source, destination;

    if (!instr.readSourceOperand (&source) || 
            !instr.readDestinationOperand (&destination))
        return false;

    u16 tmp = source & destination;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    return true;
}

bool Common::Executor::operator() (BIC& instr)
{
    CondData<u16> source, destination;

    if (!instr.readSourceOperand (&source) ||
        !instr.readDestinationOperand (&destination))
        return false;

    u16 result = ~source & destination;

    if (!instr.writeDestinationOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = false});

    return true;
}

bool Common::Executor::operator() (BICB& instr)
{
    CondData<u8> source, destination;

    if (!instr.readSourceOperand (&source) ||
            !instr.readDestinationOperand (&destination))
        return false;

    u8 tmp = (u8)(~source & destination);

    if (!instr.writeDestinationOperand (tmp))
        return false;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    return true;
}

bool Common::Executor::operator() (BIS& instr)
{
    CondData<u16> source, destination;

    if (!instr.readSourceOperand (&source) ||
            !instr.readDestinationOperand (&destination))
        return false;

    u16 tmp = source | destination;

    if (!instr.writeDestinationOperand (tmp))
        return false;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = false});

    return true;
}

bool Common::Executor::operator() (BISB& instr)
{
    CondData<u8> source, destination;

    if (!instr.readSourceOperand (&source) || 
            !instr.readDestinationOperand (&destination))
        return false;

    u8 tmp = source | destination;

    if (!instr.writeDestinationOperand (tmp))
        return false;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    return true;
}

// EIS instructions, including JSR and XOR
bool Common::Executor::operator() (JSR& instr)
{
    OperandLocation destination = 
        instr.getOperandLocation (cpuData_->registers ());

    if (!destination.isA<MemoryOperandLocation> ())
    {
        // Illegal instruction
        cpuData_->setTrap (CpuData::TrapCondition::IllegalInstructionTrap);
        return true;
    }

    GeneralRegisters& registers = cpuData_->registers ();
    u16 specifiedRegisterContents = registers[instr.getRegisterNr ()];

    if (!mmu_->pushWord (specifiedRegisterContents))
        return false;

    if (cpuData_->stackOverflow ())
        cpuData_->setTrap (CpuData::TrapCondition::StackOverflow);

    registers[instr.getRegisterNr ()] = registers[7];
    registers[7] = destination;

    return true;
}

bool Common::Executor::operator() (MUL& instr)
{
    u16 regNr = instr.getRegisterNr ();

    GeneralRegisters& registers = cpuData_->registers ();
    u16 destination = registers[regNr];

    CondData<u16> source;
    if (!instr.readOperand (&source))
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

bool Common::Executor::operator() (DIV& instr)
{
    u16 regNr = instr.getRegisterNr ();

    GeneralRegisters& registers = cpuData_->registers ();
    s32 tmps32 = (registers[regNr] << 16) | registers[regNr | 1];

    CondData<u16> source;
    if (!instr.readOperand (&source))
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

bool Common::Executor::operator() (ASH& instr)
{
    u16 regNr = instr.getRegisterNr ();
    u16 tmp {0};
    GeneralRegisters& registers = cpuData_->registers ();
    u16 destination = registers[regNr];

    CondData<u16> source;
    if (!instr.readOperand (&source))
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

bool Common::Executor::operator() (ASHC& instr)
{
    u16 regNr = instr.getRegisterNr ();
    GeneralRegisters& registers = cpuData_->registers ();
    u16 dst = registers[regNr];

    s32 tmps32 = (registers[regNr] << 16) | registers[regNr | 1];

    CondData<u16> source;
    if (!instr.readOperand (&source))
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

bool Common::Executor::operator() (XOR& instr)
{
    u16 regNr = instr.getRegisterNr ();
    GeneralRegisters& registers = cpuData_->registers ();

    u16 source = registers[regNr];

    CondData<u16> destination;
    if (!instr.readOperand (&destination))
        return false;

    u16 result = source ^ destination;

    if (!instr.writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = false});

    return true;
}


// SOB instruction format
bool Common::Executor::operator() (SOB& instr)
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
bool Common::Executor::operator() (RTS& instr)
{
    u16 regNr = instr.getRegister ();

    cpuData_->registers ()[7] = cpuData_->registers ()[regNr];
    mmu_->popWord (&cpuData_->registers ()[regNr]);

    return true;
}

// Branch instructions
bool Common::Executor::operator() (BR& instr)
{
    instr.executeBranch ();
    return true;
}

bool Common::Executor::operator() (BNE& instr)
{
    instr.executeBranchIf (!isSet (PSW_Z));
    return true;
}

bool Common::Executor::operator() (BEQ& instr)
{
    instr.executeBranchIf (isSet (PSW_Z));
    return true;
}

bool Common::Executor::operator() (BPL& instr)
{
    instr.executeBranchIf (!isSet (PSW_N));
    return true;
}

bool Common::Executor::operator() (BMI& instr)
{
    instr.executeBranchIf (isSet (PSW_N));
    return true;
}

bool Common::Executor::operator() (BVC& instr)
{
    instr.executeBranchIf (!isSet (PSW_V));
    return true;
}

bool Common::Executor::operator() (BVS& instr)
{
    instr.executeBranchIf (isSet (PSW_V));
    return true;
}

bool Common::Executor::operator() (BCC& instr)
{
    instr.executeBranchIf (!isSet (PSW_C));
    return true;
}

bool Common::Executor::operator() (BCS& instr)
{
    instr.executeBranchIf (isSet (PSW_C));
    return true;
}

bool Common::Executor::operator() (BGE& instr)
{
    instr.executeBranchIf ((isSet (PSW_N) ^ isSet (PSW_V)) == 0);
    return true;
}

bool Common::Executor::operator() (BLT& instr)
{
    instr.executeBranchIf (isSet (PSW_N) ^ isSet (PSW_V));
    return true;
}

bool Common::Executor::operator() (BGT& instr)
{
    instr.executeBranchIf ((isSet (PSW_Z) || (isSet (PSW_N) ^ isSet (PSW_V))) == 0);
    return true;
}

bool Common::Executor::operator() (BLE& instr)
{
    instr.executeBranchIf (isSet (PSW_Z) || (isSet (PSW_N) ^ isSet (PSW_V)));
    return true;
}

bool Common::Executor::operator() (BHI& instr)
{
    instr.executeBranchIf (!isSet (PSW_C) && !isSet (PSW_Z));
    return true;
}

bool Common::Executor::operator() (BLOS& instr)
{
    instr.executeBranchIf (isSet (PSW_C) || isSet (PSW_Z));
    return true;
}

bool Common::Executor::operator() (EMT& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::EmulatorTrap);
    return true;
}

bool Common::Executor::operator() (TRAP& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::TrapInstruction);
    return true;
}

// No-operand instructions
bool Common::Executor::operator() (HALT& instr)
{
    cpuControl_->halt ();
    return true;
}

bool Common::Executor::operator() (WAIT& instr)
{
    cpuControl_->wait ();
    return true;
}

// If an RTI [instruction] sets the T bit in the PS, a trace trap will occur
// before the next instruction is executed. (Micro PDP-11 Handbook, pag 322.)
// This behaviour differs from the other instructionss in which setting the
// T-bit results in a trace trap on the next instruction.
bool Common::Executor::operator() (RTI& instr)
{
    u16 tmp;

    if (!mmu_->popWord (&cpuData_->registers ()[7]) || !mmu_->popWord (&tmp))
        return false;

    cpuData_->psw ().set (PSW::ProtectionMode::RTI, tmp);
    if (cpuData_->psw ().traceBitSet ())
        cpuData_->setTrap (CpuData::TrapCondition::BreakpointTrap);

    return true;
}

bool Common::Executor::operator() (BPT& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::BreakpointTrap);
    return true;
}

bool Common::Executor::operator() (IOT& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::InputOutputTrap);
    return true;
}

bool Common::Executor::operator() (RESET& instr)
{
    cpuControl_->busReset ();
    return true;
}

//  If the RTT sets the T bit in the PS, the next instruction will be executed
// and then the trace trap will be processed. (Micro PDP-11 Handbook,
// pag 322). This is the normal behaviour for instruction setting the T-bit.
bool Common::Executor::operator() (RTT& instr)
{
    u16 tmp;

    if (!mmu_->popWord (&cpuData_->registers ()[7]) || !mmu_->popWord (&tmp))
        return false;

    cpuData_->psw ().set (PSW::ProtectionMode::RTI, tmp);

    return true;
}

// Condition Code instructions
bool Common::Executor::operator() (CCC& instr)
{
    cpuData_->psw ().set (PSW::ProtectionMode::ExplicitAccess,
        cpuData_->psw () & ~instr.getConditionCodes ());
    return true;
}

bool Common::Executor::operator() (SCC& instr)
{
    cpuData_->psw ().set (PSW::ProtectionMode::ExplicitAccess,
        cpuData_->psw () | instr.getConditionCodes ());
    return true;
}

bool Common::Executor::operator() (Unused& instr)
{
    cpuData_->setTrap (CpuData::TrapCondition::ReservedInstructionTrap);
    return true;
}

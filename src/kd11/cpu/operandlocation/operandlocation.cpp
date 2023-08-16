#include "operandlocation.h"

using std::monostate;
using std::get;
using std::holds_alternative;

// Allow an uninitialized OperandLocation to be able to define it as a
// class memeber.
OperandLocation::OperandLocation ()
    :
    location_ {monostate {}},
    cpu_ {}
{}

OperandLocation::OperandLocation (CpuData *cpu, u8 registerNumber)
    :
    location_ {registerNumber},
    cpu_ {cpu}
{}

OperandLocation::OperandLocation (CpuData *cpu, CondData<u16> memoryAddress)
    :
    location_ {memoryAddress},
    cpu_ {cpu}
{}

// Return whether or not the OperandLocation points to a valid address
bool OperandLocation::isValid ()
{
    return holds_alternative<u8> (location_) ||
        (holds_alternative<CondData <u16>> (location_) &&
            get<CondData<u16>> (location_).hasValue ());
}

// Return, depending on (the type of) the value held in the location, either
// the register number or the memory address held in the location. The type
// of the value can be determined by means of the isA<> function.
OperandLocation::operator u16 ()
{
    if (holds_alternative<u8> (location_))
        return get<u8> (location_);
     
    return get<CondData <u16>> (location_).valueOr (0);
}

// Return the contents of the location pointed to by this OperandLocation. The
// location is either a register number or a memory address. In the first case
// the contents of the register are returned, in the second case the contents
// of the memory address.
CondData<u16> OperandLocation::wordContents ()
{
    if (holds_alternative<u8> (location_))
    {
        // The variant holds a register number
        return CondData<u16> {cpu_->registers () [get<u8> (location_)]};
    }
    else
    {
        // The variant holds a memory address
        return cpu_->fetchWord (get<CondData<u16>> (location_));
    }
}

// Return the byte at the location pointed to by this OperandLocation. The
// location is either a register number or a memory address. In the first case
// the bits 0-7 of the register are returned, in the second case the byte at
// the memory address.
CondData<u8> OperandLocation::byteContents ()
{
    if (holds_alternative<u8> (location_))
    {
        // The variant holds a register number
        return CondData<u8> {static_cast<u8> 
            (cpu_->registers ()[get<u8> (location_)] & 0377)};
    }
    else
    {
        // The variant holds a memory address
        return cpu_->fetchByte (get<CondData<u16>> (location_));
    }
}

// Write the given word contents to the operand location
bool OperandLocation::write (u16 contents)
{
    if (holds_alternative<u8> (location_))
    {
        // The variant holds a register number
        cpu_->registers () [get<u8> (location_)] = contents;
        return true;
    }
    else
    {
        // The variant holds a memory address
        return cpu_->putWord (get<CondData<u16>> (location_), contents);
    }
}

// Write the given byte to the operand location
// 
// When general registers are used, byte instructions only operate on
// bits 0-7; i.e. byte 0 of the register.
//
bool OperandLocation::writeByte (u8 contents)
{
    if (holds_alternative<u8> (location_))
    {
        // The variant holds a register number
        u16 regNr = get<u8> (location_);
        cpu_->registers () [regNr] = 
            (cpu_->registers () [regNr] & 0xFF00) | contents;
        return true;
    }
    else
    {
        // The variant holds a memory address
        return cpu_->putByte (get<CondData<u16>> (location_), contents);
    }
}
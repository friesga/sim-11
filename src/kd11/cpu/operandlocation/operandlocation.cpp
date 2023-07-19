#include "operandlocation.h"

using std::monostate;
using std::get;
using std::holds_alternative;

OperandLocation::OperandLocation (KD11CPU *cpu, u8 registerNumber)
    :
    location_ {registerNumber},
    cpu_ {cpu}
{}

OperandLocation::OperandLocation (KD11CPU *cpu, CondData<u16> memoryAddress)
    :
    location_ {memoryAddress},
    cpu_ {cpu}
{}

// Return whether or not the OperandLocation is a valid address
bool OperandLocation::isValid ()
{
    return holds_alternative<u8> (location_) ||
        (holds_alternative<CondData <u16>> (location_) &&
            get<CondData<u16>> (location_).hasValue ());
}

// Return the memory address held in the location assuming the variant
// holds a CondData<u16> object and that object contains a valid address.
// ToDo: Check these conditions
OperandLocation::operator u16 ()
{
    return get<CondData <u16>> (location_).valueOr (0);
}

// Return the contents of the location pointed to by this OperandLocation. The
// location is either a register number or a memory address. In the first case
// the contents of the register are returned, in the second case the contents
// of the memory address.
u16 OperandLocation::contents ()
{
    if (holds_alternative<u8> (location_))
    {
        // The variant holds a register number
        return cpu_->register_[get<u8> (location_)];
    }
    else
    {
        // The variant holds a memory address
        return cpu_->fetchWord (get<CondData<u16>> (location_).valueOr (0));
    }
}

// Write the given contents to the operand location
void OperandLocation::write (u16 contents)
{
    if (holds_alternative<u8> (location_))
    {
        // The variant holds a register number
        cpu_->register_[get<u8> (location_)] = contents;
    }
    else
    {
        // The variant holds a memory address
        cpu_->putWord (get<CondData<u16>> (location_), contents);
    }
}
#include "operandlocation.h"

using std::monostate;
using std::get;

OperandLocation::OperandLocation ()
    :
    location_ {monostate {}}
{}

OperandLocation::OperandLocation (u8 registerNumber)
    :
    location_ {registerNumber}
{}

OperandLocation::OperandLocation (CondData<u16> memoryAddress)
    :
    location_ {memoryAddress}
{}

// Return the memory address held in the location assuming the variant
// holds a CondData<u16> object and that object contains a valid address.
// ToDo: Check these conditions
OperandLocation::operator u16 ()
{
    return get<CondData <u16>> (location_).valueOr (0);
}
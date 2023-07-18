#ifndef _OPERANDLOCATION_H_
#define _OPERANDLOCATION_H_

#include "types.h"
#include "conddata/conddata.h"
#include <variant>

using std::variant;
using std::monostate;

// A location of an instruction operand is either a register or a
// memory location. A memory locations are in the form of a CondData object.
// This has the advantage that std::variant can discriminate between
// these two types.
class OperandLocation
{
public:
    OperandLocation ();
    OperandLocation (u8 registerNumber);
    OperandLocation (CondData<u16> memoryAddress);
    template <typename T> bool isA ();
    operator u16 ();

private:
    variant <monostate,
        u8,
        CondData <u16>> location_;
};

template <typename T>
inline bool OperandLocation::isA ()
{
    return holds_alternative<T> (location_);
}

#endif // _OPERANDLOCATION_H_
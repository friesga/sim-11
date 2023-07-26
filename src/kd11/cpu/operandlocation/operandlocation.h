#ifndef _OPERANDLOCATION_H_
#define _OPERANDLOCATION_H_

#include "../kd11cpu.h"
#include <variant>

using std::variant;
using std::monostate;

// A location of an instruction operand is either a register or a memory
// location in the given CPU. A memory location is in the form of a CondData
// object. This has the advantage that std::variant can discriminate between
// these two types.
//
// The cpu parameter is used to be able to retrieve the contents of the
// location.
class OperandLocation
{
public:
    OperandLocation (KD11CPU *cpu, u8 registerNumber);
    OperandLocation (KD11CPU *cpu, CondData<u16> memoryAddress);
    template <typename T> bool isA ();
    bool isValid ();
    operator u16 ();
    CondData<u16> contents ();
    void write (u16 contents);

private:
    variant <u8, CondData <u16>> location_;
    KD11CPU *cpu_;
};

template <typename T>
inline bool OperandLocation::isA ()
{
    return holds_alternative<T> (location_);
}

#endif // _OPERANDLOCATION_H_
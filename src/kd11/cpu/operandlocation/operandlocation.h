#ifndef _OPERANDLOCATION_H_
#define _OPERANDLOCATION_H_

#include <variant>

#include "../cpudata.h"

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
    OperandLocation ();
    OperandLocation (CpuData *cpu, u8 registerNumber);
    OperandLocation (CpuData *cpu, CondData<u16> memoryAddress);
    template <typename T> bool isA ();
    bool isValid ();
    operator u16 ();
    CondData<u16> wordContents ();
    CondData<u8> byteContents ();
    bool write (u16 contents);
    bool writeByte (u8 contents);

private:
    variant <monostate, u8, CondData <u16>> location_;
    CpuData *cpu_;
};

template <typename T>
inline bool OperandLocation::isA ()
{
    return holds_alternative<T> (location_);
}

#endif // _OPERANDLOCATION_H_
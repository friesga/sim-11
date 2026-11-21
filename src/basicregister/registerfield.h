#ifndef _REGISTERFIELD_H_
#define _REGISTERFIELD_H_

#include "types.h"
#include "bitfield.h"

// A RegisterField is a named field within the value of a register. Contrary
// to a BitField it does not hold a value of its own but refers to the value
// of the register it is part of. This allows registers to be defined as
// classes instead of unions, e.g.:
//
//  class XYZ : public BasicRegister
//  {
//  public:
//      using Ready = BitField<u16, 7>;
//      RegisterField<Ready> ready {registerValue_};
//      ...
//  };
//
// The field is specified by the BitField type defining its position. That
// type can still be used to compose a register value: '+XYZ::Ready {1}'.
//
template <typename Field>
class RegisterField;

template <size_t Index, size_t Bits>
class RegisterField<BitField<u16, Index, Bits>>
{
    static_assert (Bits > 0 && Index + Bits <= 16, "Field does not fit in u16");

public:
    static constexpr u16 mask = static_cast<u16> (((1u << Bits) - 1u) << Index);

    // Bind the field to the value of the register it is part of. The field
    // has no value of its own; it reads and writes its bits in that value.
    explicit RegisterField (u16& value) : registerValue_ {value} {}

    // A field cannot be copied as it refers to the value of its register
    RegisterField (RegisterField const&) = delete;
    RegisterField& operator= (RegisterField const&) = delete;

    operator u16 () const
    {
        return static_cast<u16> ((registerValue_ & mask) >> Index);
    }

    RegisterField& operator= (u16 value)
    {
        registerValue_ = static_cast<u16> ((registerValue_ & ~mask) | ((value << Index) & mask));
        return *this;
    }

    // Return the field's bits at their position in the register value
    u16 operator+ () const { return static_cast<u16> (registerValue_ & mask); }

    RegisterField& operator++ () { return *this = static_cast<u16> (*this + 1); }
    u16 operator++ (int) { u16 r = *this; ++*this; return r; }
    RegisterField& operator-- () { return *this = static_cast<u16> (*this - 1); }
    u16 operator-- (int) { u16 r = *this; --*this; return r; }

private:
    u16& registerValue_;
};

#endif // _REGISTERFIELD_H_
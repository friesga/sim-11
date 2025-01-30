#ifndef _BITFIELD_H_
#define _BITFIELD_H_

#include "types.h"

// This class implements portable bit fields for a specific underlying
// type, in this case u16. When combined with a union the individual bits
// and fields as well as the value as a u16 can be set and read. This can
// be used e.g. to set and read register values.
//
// The implementation is based on 
// https://blog.codef00.com/2014/12/06/portable-bitfields-using-c11
// with the following modifications:
// - The template MinimumTypeHelper is replaced by a fixed value type (u16)
//   to prevent a combination of different underlying value types and
//   thus make the implementation more portable,
// - Renamed BitField to U16BitField to emphasize the underlying type. This
//   prevents confusion with the bitField expression and avoids confusion
//   with a possible BitField implementation for u32.
//
template <size_t Index, size_t Bits = 1>
class U16BitField
{
private:
    enum
    {
        Mask = (1u << Bits) - 1u
    };

public:
    template <class T>
    U16BitField& operator=(T value)
    {
        value_ = (value_ & ~(Mask << Index)) | ((value & Mask) << Index);
        return *this;
    }

    using value_type = u16;

    operator value_type () const { return (value_ >> Index) & Mask; }
    explicit operator bool () const { return value_ & (Mask << Index); }
    U16BitField& operator++() { return *this = *this + 1; }
    value_type operator++(int) { value_type r = *this; ++*this; return r; }
    U16BitField& operator--() { return *this = *this - 1; }
    value_type operator--(int) { value_type r = *this; --*this; return r; }

private:
    value_type value_;
};

template <size_t Index>
class U16BitField<Index, 1>
{
private:
    enum
    {
        Bits = 1,
        Mask = 0x01
    };

    using value_type = u16;

public:
    U16BitField& operator=(bool value)
    {
        value_ = (value_ & ~(Mask << Index)) | (value << Index);
        return *this;
    }

    explicit operator bool () const { return value_ & (Mask << Index); }

private:
    value_type value_;
};

#endif // _BITFIELD_H_
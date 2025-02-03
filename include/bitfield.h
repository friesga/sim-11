#ifndef _BITFIELD_H_
#define _BITFIELD_H_

#include "types.h"

// This class implements portable bit fields for a specific underlying
// type. When combined with a union the individual bits and fields as well
// as the value as the underlying type can be set and read. This can be
// used e.g. to set and read register values.
//
// The implementation is based on 
// https://blog.codef00.com/2014/12/06/portable-bitfields-using-c11
// with the following modifications:
// - The template MinimumTypeHelper is replaced by a fixed value type
//   to prevent a combination of different underlying value types and
//   thus make the implementation more portable,
// - Templated BitField on the underlying value type.
//
template <typename T, size_t Index, size_t Bits = 1>
class BitField
{
private:
    enum
    {
        Mask = (1u << Bits) - 1u
    };

public:
    BitField& operator= (T value)
    {
        value_ = (value_ & ~(Mask << Index)) | ((value & Mask) << Index);
        return *this;
    }

    operator T () const { return (value_ >> Index) & Mask; }
    operator bool () const { return value_ & (Mask << Index); }
    BitField& operator++() { return *this = *this + 1; }
    T operator++(int) { T r = *this; ++*this; return r; }
    BitField& operator--() { return *this = *this - 1; }
    T operator--(int) { T r = *this; --*this; return r; }

private:
    T value_;
};

template <typename T, size_t Index>
class BitField<T, Index, 1>
{
private:
    enum
    {
        Bits = 1,
        Mask = 0x01
    };

public:
    BitField& operator=(bool value)
    {
        value_ = (value_ & ~(Mask << Index)) | (value << Index);
        return *this;
    }

    operator bool () const { return value_ & (Mask << Index); }

private:
    T value_;
};

#endif // _BITFIELD_H_
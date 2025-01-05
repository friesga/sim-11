#ifndef _BUSADDRESS_H_
#define _BUSADDRESS_H_

// Replacements for include of types.h
#include <cstdint>

using u16 = uint16_t;
using u32 = uint32_t;

#include <array>

using std::array;

// This class abstracts a bus address with the given width, i.e. the number
// of address bits. Regular PDP-11 values are:
// - 16-bit for an unmapped physical address,
// - 18-bit for an Unibus address or 18-bit Qbus address,
// - 22-bit for a 22-bit Qbus address.
//
template <int width = 16>
class BusAddress
{
public:
    BusAddress (u32 value);
    operator u32 ();
    BusAddress& operator += (u32 value);
    BusAddress& operator &= (u32 mask);
    bool operator== (BusAddress other) const;
    bool isInIOpage ();
    bool isInIOpageLower2K ();
    u16 registerAddress ();

    static const size_t addressWidth = width;

private:
    u32 value_;
    u32 maxNumber_ {(1 << width) - 1};
    u32 ioPageBase_ {maxNumber_ - (8 * 1024) + 1};
};

// Initialize the bus address with the given value, taking into consideration
// the maximum number that can be represented with the given width. This
// implies that a number greater than the maximum number for that width will
// overflow from zero. This feature is verified by the JKDAD1 diagnostic
// test 24.
template <int width>
BusAddress<width>::BusAddress (u32 value)
{
    value_ = value & maxNumber_;
}

template <int width>
BusAddress<width>::operator u32 ()
{
    return value_;
}

template <int width>
BusAddress<width>& BusAddress<width>::operator += (u32 value)
{
    value_ += value;
    return *this;
}

template <int width>
BusAddress<width>& BusAddress<width>::operator &= (u32 mask)
{
    value_ &= mask;
    return *this;
}

template <int width>
bool BusAddress<width>::isInIOpage ()
{
    return (value_ & ioPageBase_) == ioPageBase_;
}

template <int width>
bool BusAddress<width>::isInIOpageLower2K ()
{
    u32 lower2KioPageBase = ioPageBase_ + 4096;
    return (value_ & lower2KioPageBase) == lower2KioPageBase;
}

// Return the bus address as a register address, i.e. an u16 address of
// a register in the I/O space.
template <int width>
u16 BusAddress<width>::registerAddress ()
{
    return value_ & 0177777;
}

template <int width>
BusAddress<width> operator& (BusAddress<width> lhs, u32 rhs)
{
    lhs &= rhs;
    return lhs;
}

template <int width>
bool BusAddress<width>::operator== (BusAddress<width> other) const
{
    return value_ == other.value_;
}

// Valid bus addresses are 16, 18 or 22 bits wide.
template <typename T>
concept ValidBusAddress =
    std::same_as<T, BusAddress<16>> ||
    std::same_as<T, BusAddress<18>> ||
    std::same_as<T, BusAddress<22>>;
#endif // _BUSADDRESS_H_
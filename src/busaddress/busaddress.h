#ifndef _BUSADDRESS_H_
#define _BUSADDRESS_H_

#include "types.h"

#include <array>
#include <cassert>

using std::array;

// This class abstracts a bus address with the given width, i.e. the number
// of address bits. Regular PDP-11 values are:
// - 16-bit for an unmapped physical address,
// - 18-bit for an Unibus address or 18-bit Qbus address,
// - 22-bit for a 22-bit Qbus address or Extended Unibus (EUB).
//
// I/O page base addresses:
// - 16-bit address: 0160000
// - 18-bit address: 0760000
// - 22-bit address: 017760000
//
class BusAddress
{
public:
    enum class Width
    {
        _16Bit = 16,
        _18Bit = 18,
        _22Bit = 22
    };

    BusAddress (u32 value);
    BusAddress (u32 value, Width width);
    Width width ();
    operator u32 ();
    BusAddress& operator += (u32 value);
    BusAddress& operator &= (u32 mask);
    bool isInIOpage ();
    bool isInIOpageLower2K ();
    u16 registerAddress ();

private:
    Width width_;
    u32 value_;
    u32 maxNumber_;
    u32 ioPageBase_;
};


// Initialize the bus address with the given value, taking into consideration
// the maximum number that can be represented with the given width. This
// implies that a number greater than the maximum number for that width will
// overflow from zero. This feature is verified by the JKDAD1 diagnostic
// test 24.
inline BusAddress::BusAddress (u32 value, Width width)
{
    width_ = width;
    maxNumber_ = (1 << static_cast<u32> (width)) - 1;
    ioPageBase_ = maxNumber_ - (8 * 1024) + 1;
    value_ = value & maxNumber_;
}

inline BusAddress::BusAddress (u32 value)
    :
    BusAddress (value, Width::_16Bit)
{
}

inline BusAddress::Width BusAddress::width ()
{
    return width_;
}

inline BusAddress::operator u32 ()
{
    return value_;
}

inline BusAddress& BusAddress::operator += (u32 value)
{
    value_ += value;
    return *this;
}

inline BusAddress& BusAddress::operator &= (u32 mask)
{
    value_ &= mask;
    return *this;
}

inline bool BusAddress::isInIOpage ()
{
    return (value_ & ioPageBase_) == ioPageBase_;
}

inline bool BusAddress::isInIOpageLower2K ()
{
    u32 lower2KioPageBase = ioPageBase_ + 4096;
    return (value_ & lower2KioPageBase) == lower2KioPageBase;
}

// Return the bus address as a register address, i.e. an u16 address of
// a register in the I/O space. To prevent future issues assert that the
// address actually is in the I/O space.
inline u16 BusAddress::registerAddress ()
{
    assert (isInIOpage ());
    return value_ & 0177777;
}

inline BusAddress operator& (BusAddress lhs, u32 rhs)
{
    lhs &= rhs;
    return lhs;
}

#endif // _BUSADDRESS_H_
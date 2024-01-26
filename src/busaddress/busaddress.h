#ifndef _BUSADDRESS_H_
#define _BUSADDRESS_H_

#include "types.h"

#include <array>

using std::array;

// This class abstracts a 16-, 18- or 22-bit bus address.
class BusAddress
{
public:
    enum class Width
    {
        _16Bit = 0,
        _18Bit = 1,
        _22Bit = 2
    };

    BusAddress (u32 value);
    BusAddress (u32 value, Width width);
    operator u32 ();
    BusAddress& operator += (u32 value);
    BusAddress& operator &= (u32 mask);
    bool isInIOpage ();
    bool isInIOpageLower2K ();
    u16 registerAddress ();

private:
    u32 value_;
    u32 ioPageBase_;
    static const array<u32, 3> maxNumber_;
    static const array<u32, 3> ioPageBases_;
};

// Definition of the used bits for 16-, 18- and 22-bits wide busses.
inline const array<u32, 3>  BusAddress::maxNumber_ =
{
    0177777,    // 16-bit
    0777777,    // 18-bit
    017777777   // 22-bit
};

// Definition of the I/O page base address for 16-, 18- and 22-bits
// wide busses. These constants could be derived from the maxNumber_
// constants (maxNumber_[x] - (8 * 1024) + 1) but we'll keep them as
// separate constants for reference puposes.
inline const array<u32, 3>  BusAddress::ioPageBases_ =
{
    0160000,    // 16-bit
    0760000,    // 18-bit
    017760000   // 22-bit
};


// Initialize the bus address with the given value, taking into consideration
// the maximum number that can be represented with the given width. This
// implies that a number greater than the maximum number for that width will
// overflow from zero. This feature is verified by the JKDAD1 diagnostic
// test 24.
inline BusAddress::BusAddress (u32 value, Width width)
{
    value_ = value & maxNumber_.at (static_cast<size_t> (width));
    ioPageBase_ = ioPageBases_.at (static_cast<size_t> (width));
}

inline BusAddress::BusAddress (u32 value)
    :
    BusAddress (value, Width::_16Bit)
{}

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
// a register in the I/O space.
inline u16 BusAddress::registerAddress ()
{
    return value_ & 0177777;
}

inline BusAddress operator& (BusAddress lhs, u32 rhs)
{
    lhs &= rhs;
    return lhs;
}

#endif // _BUSADDRESS_H_
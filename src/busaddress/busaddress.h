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
    BusAddress& operator &= (u16 mask);
    bool isInIOpage ();
    u16 registerAddress ();

private:
    u32 value_;
    u32 ioPageBase_;
    static const array<u32, 3> ioPageBases_;
};

// Definition of the I/O page base address for 16-, 18- and 22-bits wide bus.
inline const array<u32, 3>  BusAddress::ioPageBases_ =
{
    016000,     // 16-bit
    0760000,    // 18-bit
    017760000   // 22-bit
};


inline BusAddress::BusAddress (u32 value, Width width)
{
    value_ = value;
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


inline BusAddress& BusAddress::operator &= (u16 mask)
{
    value_ &= mask;
    return *this;
}

inline bool BusAddress::isInIOpage ()
{
    return value_ & ioPageBase_;
}

// Return the bus address as a register address, i.e. an u16 address of
// a register in the I/O space.
inline u16 BusAddress::registerAddress ()
{
    return value_ & 0177777;
}


#endif // _BUSADDRESS_H_
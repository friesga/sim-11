#ifndef _BUSADDRESS_H_
#define _BUSADDRESS_H_

#include "types.h"

// This class abstracts a 16-, 18- or 22-bit bus address.
class BusAddress
{
public:
    BusAddress (u16 value);
    operator u16 ();
    BusAddress& operator &= (u16 mask);
    u16 registerAddress ();

private:
    u16 value_;
};

inline BusAddress::BusAddress (u16 value)
{
    value_ = value;
}

inline BusAddress::operator u16 ()
{
    return value_;
}

inline BusAddress& BusAddress::operator &= (u16 mask)
{
    value_ &= mask;
    return *this;
}

// Return the bus address as a register address, i.e. an u16 address of
// a register in the I/O space.
inline u16 BusAddress::registerAddress ()
{
    return value_;
}

#endif // _BUSADDRESS_H_
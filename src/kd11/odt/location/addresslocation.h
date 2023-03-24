#ifndef _ADDRESSLOCATION_H_
#define _ADDRESSLOCATION_H_

#include <cstdint>

using u16 = uint16_t;

class AddressLocation
{
public:
    AddressLocation (u16 address);
    u16 address ();

private:
    u16 address_;
};

inline AddressLocation::AddressLocation (u16 address)
    :
    address_ {address}
{}

inline u16 AddressLocation::address ()
{
    return address_;
}

#endif // _ADDRESSLOCATION_H_
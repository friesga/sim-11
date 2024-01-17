#ifndef _ADDRESSLOCATION_H_
#define _ADDRESSLOCATION_H_

#include <cstdint>

using u16 = uint16_t;

// This class keeps track of an address location. The class discriminates
// between the address as input by the user and the derived word address.
// The input address might be a byte address, the word address is the
// input address with bit 0 cleared.
class AddressLocation
{
public:
    AddressLocation (u16 address);
    u16 inputAddress ();
    u16 wordAddress ();

private:
    u16 address_;
};

inline AddressLocation::AddressLocation (u16 address)
    :
    address_ {address}
{}

inline u16 AddressLocation::inputAddress ()
{
    return address_;
}

inline u16 AddressLocation::wordAddress ()
{
    return address_ & 0177776;
}

#endif // _ADDRESSLOCATION_H_
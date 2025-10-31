#ifndef _ADDRESSLOCATION_H_
#define _ADDRESSLOCATION_H_

#include "types.h"

#include <limits>
#include <concepts>

using std::numeric_limits;
using std::integral;

// This class keeps track of an address location. The class discriminates
// between the address as input by the user and the derived word address.
// The input address might be a byte address, the word address is the
// input address with bit 0 cleared.
template <integral AddressType>
class AddressLocation
{
public:
    AddressLocation (AddressType address);
    AddressType inputAddress ();
    AddressType wordAddress ();

private:
    AddressType address_;
};

template <integral AddressType>
inline AddressLocation<AddressType>::AddressLocation (AddressType address)
    :
    address_ {address}
{}

template <integral AddressType>
inline AddressType AddressLocation<AddressType>::inputAddress ()
{
    return address_;
}

template <integral AddressType>
inline AddressType AddressLocation<AddressType>::wordAddress ()
{
    return address_ & (numeric_limits<AddressType>::max () - 1);
}

#endif // _ADDRESSLOCATION_H_
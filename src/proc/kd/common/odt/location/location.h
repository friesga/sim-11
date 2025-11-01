#ifndef _OPENLOCATION_H_
#define _OPENLOCATION_H_

#include "addresslocation.h"
#include "registerlocation.h"
#include "pswlocation.h"

#include <concepts>
#include <variant>
#include <utility>

using std::variant;
using std::monostate;
using std::get;
using std::holds_alternative;
using std::move;
using std::integral;

template <integral AddressType>
class Location
{
public:
    using LocationVariant = variant<monostate,
        AddressLocation<AddressType>,
        RegisterLocation,
        PSWLocation>;

    Location ();
    template <typename LocationType> LocationType opened ();
    template <typename LocationType> LocationType previous ();
    template <typename LocationType> bool isA ();
    template <typename LocationType> bool previousIsA ();
    Location &operator= (const LocationVariant &location);
    AddressType inputAddress ();
    AddressType wordAddress ();
    u8 registerNr ();
    AddressType previousInputAddress ();
    AddressType previousWordAddress ();
    u8 previousRegisterNr ();

private:
    LocationVariant openedLocation_;
    LocationVariant previousLocation_;
};

template <integral AddressType>
inline Location<AddressType>::Location ()
    :
    openedLocation_ {monostate {}},
    previousLocation_ {monostate {}}
{}

// Save the current location object and assign a new location object to the
// current location.
template <integral AddressType>
inline Location<AddressType> &Location<AddressType>::operator= (const LocationVariant &location)
{
    previousLocation_ = move (openedLocation_);
    openedLocation_ = location;
    return *this;
}

template <integral AddressType>
template <typename LocationType>
inline LocationType Location<AddressType>::opened ()
{
    return get<LocationType> (openedLocation_);
}

template <integral AddressType>
template <typename LocationType>
inline LocationType Location<AddressType>::previous ()
{
    return get<LocationType> (previousLocation_);
}

template <integral AddressType>
template <typename LocationType>
inline bool Location<AddressType>::isA ()
{
    return holds_alternative<LocationType> (openedLocation_);
}

template <integral AddressType>
template <typename LocationType>
inline bool Location<AddressType>::previousIsA ()
{
    return holds_alternative<LocationType> (previousLocation_);
}


// Return the input address, presuming the current location is
// an AddressLocation
template <integral AddressType>
inline AddressType Location<AddressType>::inputAddress ()
{
    return get<AddressLocation<AddressType>> (openedLocation_).inputAddress ();
}

// Return the currently opened address, presuming the current location is
// an AddressLocation
template <integral AddressType>
inline AddressType Location<AddressType>::wordAddress ()
{
    return get<AddressLocation<AddressType>> (openedLocation_).wordAddress ();
}

// Return the currently opened register, presuming the current location is
// a RegisterLocation
template <integral AddressType>
inline u8 Location<AddressType>::registerNr ()
{
    return get<RegisterLocation> (openedLocation_).registerNr ();
}

// Return the previous input address, presuming the previous location is
// an AddressLocation
template <integral AddressType>
inline AddressType Location<AddressType>::previousInputAddress ()
{
    return get<AddressLocation<AddressType>> (previousLocation_).inputAddress ();
}

// Return the previously opened address, presuming the previous location is
// an AddressLocation
template <integral AddressType>
inline AddressType Location<AddressType>::previousWordAddress ()
{
    return get<AddressLocation<AddressType>> (previousLocation_).wordAddress ();
}

// Return the previously opened register, presuming the previous location is
// a RegisterLocation
template <integral AddressType>
inline u8 Location<AddressType>::previousRegisterNr ()
{
    return get<RegisterLocation> (previousLocation_).registerNr ();
}
#endif // _OPENLOCATION_H_
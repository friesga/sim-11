#ifndef _OPENLOCATION_H_
#define _OPENLOCATION_H_

#include "addresslocation.h"
#include "registerlocation.h"
#include "pswlocation.h"

#include <variant>
#include <utility>

using std::variant;
using std::monostate;
using std::get;
using std::holds_alternative;
using std::move;

class Location
{
public:
    using LocationVariant = variant<monostate,
        AddressLocation,
        RegisterLocation,
        PSWLocation>;

    Location ();
    template <typename LocationType> LocationType opened ();
    template <typename LocationType> LocationType previous ();
    template <typename LocationType> bool isA ();
    template <typename LocationType> bool previousIsA ();
    Location &operator= (const LocationVariant &location);
    u16 inputAddress ();
    u16 wordAddress ();
    u8 registerNr ();
    u16 previousInputAddress ();
    u16 previousWordAddress ();
    u8 previousRegisterNr ();

private:
    LocationVariant openedLocation_;
    LocationVariant previousLocation_;
};

inline Location::Location ()
    :
    openedLocation_ {monostate {}},
    previousLocation_ {monostate {}}
{}

// Save the current location object and assign a new location object to the
// current location. 
inline Location &Location::operator= (const LocationVariant &location)
{
    previousLocation_ = move (openedLocation_);
    openedLocation_ = location;
    return *this;
}

template <typename LocationType>
inline LocationType Location::opened ()
{
    return get<LocationType> (openedLocation_);
}

template <typename LocationType>
inline LocationType Location::previous ()
{
    return get<LocationType> (previousLocation_);
}

template <typename LocationType>
inline bool Location::isA ()
{
    return holds_alternative<LocationType> (openedLocation_);
}

template <typename LocationType>
inline bool Location::previousIsA ()
{
    return holds_alternative<LocationType> (previousLocation_);
}


// Return the input address, presuming the current location is
// an AddressLocation
inline u16 Location::inputAddress ()
{
    return get<AddressLocation> (openedLocation_).inputAddress ();
}

// Return the currently opened address, presuming the current location is
// an AddressLocation
inline u16 Location::wordAddress ()
{
    return get<AddressLocation> (openedLocation_).wordAddress ();
}

// Return the currently opened register, presuming the current location is
// a RegisterLocation
inline u8 Location::registerNr ()
{
    return get<RegisterLocation> (openedLocation_).registerNr ();
}

// Return the previous input address, presuming the previous location is
// an AddressLocation
inline u16 Location::previousInputAddress ()
{
    return get<AddressLocation> (previousLocation_).inputAddress ();
}

// Return the previously opened address, presuming the previous location is
// an AddressLocation
inline u16 Location::previousWordAddress ()
{
    return get<AddressLocation> (previousLocation_).wordAddress ();
}

// Return the previously opened register, presuming the previous location is
// a RegisterLocation
inline u8 Location::previousRegisterNr ()
{
    return get<RegisterLocation> (previousLocation_).registerNr ();
}
#endif // _OPENLOCATION_H_
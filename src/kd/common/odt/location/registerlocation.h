#ifndef _REGISTERLOCATION_H_
#define _REGISTERLOCATION_H_

#include <cstdint>

using u8 = uint8_t;

class RegisterLocation
{
public:
    RegisterLocation (u8 registerNr);
    u8 registerNr ();

private:
    u8 registerNr_;
};

inline RegisterLocation::RegisterLocation (u8 registerNr)
    :
    registerNr_ {registerNr}
{}

inline u8 RegisterLocation::registerNr ()
{
    return registerNr_;
}

#endif // _REGISTERLOCATION_H_
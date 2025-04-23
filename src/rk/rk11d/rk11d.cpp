#include "rk11d.h"


// This constructor is meant to be used in unit tests that don't
// require a window. It's also called by the regular constructor
// to define common actions in just one place.
RK11D::RK11D (Bus* bus, RK11DConfig& rk11dConfig)
    :
    PDP11Peripheral (bus)
{
    baseAddress_  = rk11dConfig.address;
}

RK11D::RK11D (Bus* bus, Window* window, RK11DConfig& rk11dConfig)
    :
    RK11D (bus, rk11dConfig)
{}

CondData<u16> RK11D::read (BusAddress address)
{
    return 0;
}


StatusCode RK11D::writeWord (BusAddress address, u16 value)
{
    return StatusCode::Success;
}

bool RK11D::responsible (BusAddress address)
{
    if (!address.isInIOpage ())
        return false;

    return address.registerAddress () >= baseAddress_ &&
        address.registerAddress () <= (baseAddress_ + RKDB) ?
        true : false;
}

void RK11D::reset ()
{ }
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

StatusCode RK11D::writeWord (BusAddress busAddress, u16 value)
{
    return StatusCode::Success;
}

bool RK11D::responsible (BusAddress busAddress)
{
    if (!busAddress.isInIOpage ())
        return false;

    return busAddress.registerAddress () >= baseAddress_ &&
        busAddress.registerAddress () <= (baseAddress_ + RKDB) ?
        true : false;
}

void RK11D::reset ()
{ }
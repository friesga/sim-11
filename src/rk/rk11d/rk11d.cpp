#include "rk11d.h"

using std::make_unique;

// This constructor can be called in the initialization of both sim-11 and
// the unit tests. In the latter case the window is not available.
//
RK11D::RK11D (Bus* bus, Window* window, shared_ptr<RK11DConfig> rk11dConfig)
    :
    PDP11Peripheral (bus)
{
    baseAddress_  = rk11dConfig->address;

    for (auto rk05Config : rk11dConfig->rk05Config)
    {
        if (rk05Config != nullptr)
            rk05Drives_.push_back (make_unique<RK05> (bus, this, window, rk05Config));
    }
}

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
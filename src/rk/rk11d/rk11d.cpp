#include "rk11d.h"

#include <functional>
#include <mutex>

using std::bind;
using std::make_unique;
using std::mutex;
using std::lock_guard;
using std::placeholders::_1;

// This constructor can be called in the initialization of both sim-11 and
// the unit tests. In the latter case the window is not available.
//
RK11D::RK11D (Bus* bus, Window* window, shared_ptr<RK11DConfig> rk11dConfig)
    :
    PDP11Peripheral (bus),
    bus_ {bus}
{
    baseAddress_  = rk11dConfig->address;

    for (auto rk05Config : rk11dConfig->rk05Config)
    {
        if (rk05Config != nullptr)
            rk05Drives_.push_back (make_unique<RK05> (bus, this, window, rk05Config));
    }

    bus_->BINIT ().subscribe (bind (&RK11D::BINITReceiver, this, _1));

    // Initialize the controller
    reset ();
}

bool RK11D::responsible (BusAddress busAddress)
{
    if (!busAddress.isInIOpage ())
        return false;

    return busAddress.registerAddress () >= baseAddress_ &&
        busAddress.registerAddress () <= (baseAddress_ + RKDB) ?
        true : false;
}

// RK11-D device reset 
//
void RK11D::reset ()
{
    // Clear all RKCS bits and set Controller Ready.
    rkcs_.value = 0;
    rkcs_.controlReady = 1;
}

// On assertion of the BINIT signal initialize the device.
void RK11D::BINITReceiver (bool signalValue)
{
    if (signalValue)
    {
        // Guard against controller register access from writeWord()
        lock_guard<mutex> lock {controllerMutex_};

        reset ();
    }
}
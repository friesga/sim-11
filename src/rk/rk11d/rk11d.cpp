#include "rk11d.h"

#include <functional>
#include <mutex>

using std::bind;
using std::make_unique;
using std::mutex;
using std::lock_guard;
using std::placeholders::_1;
using std::thread;

//
// This constructor can be called in the initialization of both sim-11 and
// the unit tests. In the latter case the window is not available and is a
// nullptr.
//
RK11D::RK11D (Bus* bus, Window* window, shared_ptr<RK11DConfig> rk11dConfig)
    :
    bus_ {bus}
{
    baseAddress_  = rk11dConfig->address;

    for (auto rk05Config : rk11dConfig->rk05Config)
    {
        if (rk05Config != nullptr)
            rk05Drives_.push_back (make_unique<RK05> (bus, this, window, rk05Config));
    }

    bus_->BINIT ().subscribe (bind (&RK11D::BINITReceiver, this, _1));

    // Start the action processor
    running_ = true;
    actionProcessorThread_ = thread (&RK11D::actionProcessor, this);

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

// RK11-D controller reset 
// 
// This function is executed on a BUS INIT and the receipt of a Control[ler]
// Reset function.
// 
// The Control Reset function initializes all internal registers and
// flip-flops and clears all of the bits of the seven programmable registers
// except RKCS 07 (READY), which it sets and RKDS 01 through 11, which are
// not affected. (EK-RK11D-MM-002, par. 1.3.2.1)
//
void RK11D::reset ()
{
    rkds_.drivePowerLow = 0;
    rkds_.driveId = 0;
    rker_.value = 0;
    rkcs_.value = 0;
    rkcs_.controlReady = 1;
    rkwc_ = 0;
    rkba_ = 0;
    rkda_.value = 0;
    rkdb_ = 0;
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
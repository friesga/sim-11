#include "ka11.h"

#include <memory>

using std::make_unique;

KA11::KA11 (Bus* bus, Window* window, const KA11Config& ka11Config)
    :
    bus_ {bus}
{
    ky11_a_ = make_unique<KY11_A> (bus, &cpuController_, window,
        *ka11Config.ky11_aConfig_);

    vector<BusDevice*> devices {&cpuData_, ky11_a_.get ()};
    registerHandler_ = make_unique<RegisterHandler> (devices);

    machineState_ = make_unique<KA11MachineState> (bus_, &cpuData_,
        &cpuController_, &dataPaths_, *ky11_a_);
}

KA11::~KA11 ()
{
    machineState_->exit ();
    ka11Thread_.join ();
}

void KA11::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}

void KA11::start ()
{
    ka11Thread_ = thread ([&, this] {machineState_->run (); });
}

CondData<u16> KA11::read (BusAddress address)
{
    return registerHandler_->read (address);
}

StatusCode KA11::writeWord (BusAddress address, u16 value)
{
    return registerHandler_->writeWord (address, value);
}

bool KA11::responsible (BusAddress address)
{
    return registerHandler_->responsible (address);
}

void KA11::reset ()
{
    return registerHandler_->reset ();
}

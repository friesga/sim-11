#include "ka11.h"

#include <memory>

using std::make_unique;

KA11::KA11 (Bus* bus, Window* window, const KA11Config& ka11Config)
    :
    bus_ {bus}
{
    machineState_ = make_unique<KA11MachineState> (bus_, &cpuData_,
        &cpuController_, &mmu_);

    ky11_a_ = make_unique<KY11_A> (bus, &cpuController_, window,
        *ka11Config.ky11_aConfig_);
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
    return {};
}

StatusCode KA11::writeWord (BusAddress address, u16 value)
{
    return StatusCode::FunctionNotImplemented;
}

StatusCode KA11::writeByte (BusAddress address, u8 value)
{
    return StatusCode::FunctionNotImplemented;
}

bool KA11::responsible (BusAddress address)
{
    return false;
}

void KA11::reset ()
{}

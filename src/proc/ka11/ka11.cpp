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

void KA11::start (u16 startAddress)
{ }

void KA11::start ()
{ }

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

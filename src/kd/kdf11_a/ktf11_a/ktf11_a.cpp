#include "ktf11_a.h"

KTF11_A::KTF11_A (Qbus* bus)
    :
    bus_ {bus}
{}

CondData<u16> KTF11_A::read (u16 address)
{
    return bus_->read (address);
}

bool KTF11_A::writeWord (u16 address, u16 value)
{
    return bus_->writeWord (address, value);
}


bool KTF11_A::writeByte (u16 address, u8 value)
{
    return bus_->writeByte (address, value);
}

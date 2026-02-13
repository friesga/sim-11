#include "businterface.h"

BusInterface::BusInterface (Bus* bus)
    :
    bus_ {bus}
{}

CondData<u16> BusInterface::read (BusAddress address)
{
    return bus_->read (address);
}

bool BusInterface::writeWord (BusAddress address, u16 value)
{
    return bus_->writeWord (address, value);
}

bool BusInterface::writeByte (BusAddress address, u8 value)
{
    return bus_->writeByte (address, value);

}
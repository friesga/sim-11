#include "businterface.h"

BusInterface::BusInterface (Bus* bus)
    :
    bus_ {bus}
{}

void BusInterface::attachConsole (KY11Console* console)
{
    console_ = console;
}

CondData<u16> BusInterface::read (BusAddress address)
{
    CondData<u16> data = bus_->read (address);

    if (console_ != nullptr) 
        console_->display (address, data);

    return data;
}

bool BusInterface::writeWord (BusAddress address, u16 value)
{
    bool result = bus_->writeWord (address, value);

    if (result && console_ != nullptr)
        console_->display (address, value);

    return result;
}

bool BusInterface::writeByte (BusAddress address, u8 value)
{
    bool result = bus_->writeByte (address, value);

    if (result && console_ != nullptr)
        console_->display (address, value);

    return result;
}
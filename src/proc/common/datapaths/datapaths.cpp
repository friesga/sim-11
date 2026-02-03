#include "datapaths.h"


DataPaths::DataPaths (Bus* bus, MMU* mmu)
	: 
    bus_ {bus},
    mmu_ {mmu}
{}

void DataPaths::reset ()
{}

CondData<u16> DataPaths::fetchWord (VirtualAddress address,
	PSW::Mode memMgmtMode)
{
	if (mmu_ != nullptr)
        return mmu_->fetchWord (address, memMgmtMode);
    else
        return bus_->read (address);
}

CondData<u8> DataPaths::fetchByte (VirtualAddress address,
    PSW::Mode memMgmtMode)
{
    CondData<u16> retValue {};
    if (address & 1)
    {
        retValue = fetchWord (address & 0xFFFE);
        if (retValue.hasValue ())
            return CondData<u8> (retValue >> 8);
    }
    else
    {
        retValue = fetchWord (address);
        if (retValue.hasValue ())
            return CondData<u8> (retValue & 0377);
    }

    return CondData<u8> {};
}

bool DataPaths::putWord (VirtualAddress address, u16 value,
	PSW::Mode memMgmtMode)
{
    if (mmu_ != nullptr)
        return mmu_->putWord (address, value, memMgmtMode);
    else
        return bus_->writeWord (address, value);
}

bool DataPaths::putByte (VirtualAddress address, u8 value,
	PSW::Mode memMgmtMode)
{
    if (mmu_ != nullptr)
        return mmu_->putByte (address, value, memMgmtMode);
    else
        return bus_->writeByte (address, value);
}

bool DataPaths::pushWord (u16 value)
{
    if (mmu_ != nullptr)
        return mmu_->pushWord (value);
}

bool DataPaths::popWord (u16* destination)
{
    if (mmu_ != nullptr)
        return mmu_->popWord (destination);
}

void DataPaths::setVirtualPC (u16 value)
{
    if (mmu_ != nullptr)
        mmu_->setVirtualPC (value);
}

CondData<u16> DataPaths::readWithoutTrap (u16 address)
{
    if (mmu_ != nullptr)
        return mmu_->readWithoutTrap (address);
}

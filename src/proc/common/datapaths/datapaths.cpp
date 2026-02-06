#include "datapaths.h"

DataPaths::DataPaths (Bus* bus, MMU* mmu)
    :
    bus_ {bus},
    mmu_ {mmu}
{
}

void DataPaths::reset ()
{
    if (mmu_ != nullptr)
        mmu_->reset ();

    bus_->reset ();
}

CondData<u16> DataPaths::fetchWord (VirtualAddress address,
    PSW::Mode memMgmtMode)
{
    if (mmu_ != nullptr)
        return mmu_->fetchWord (address, memMgmtMode);
    else
        return bus_->read (address);
}

// Fetch the byte at the given word or byte address
//
// The validity of the fetched word has to be checked before the shift-
// and and-operators can be applied to the word!
CondData<u8> DataPaths::fetchByte (VirtualAddress address,
    PSW::Mode memMgmtMode)
{
    CondData<u16> retValue {};
    if (address & 1)
    {
        retValue = fetchWord (address & 0xFFFE, memMgmtMode);
        if (retValue.hasValue ())
            return CondData<u8> (retValue >> 8);
    }
    else
    {
        retValue = fetchWord (address, memMgmtMode);
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
    return mmu_->pushWord (value);
}

bool DataPaths::popWord (u16* destination)
{
    return mmu_->popWord (destination);
}

void DataPaths::setVirtualPC (u16 value)
{
    mmu_->setVirtualPC (value);
}

CondData<u16> DataPaths::readWithoutTrap (u16 address)
{
    return mmu_->readWithoutTrap (address);
}

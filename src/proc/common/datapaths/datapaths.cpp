#include "datapaths.h"

#include <utility>

using std::bind;
using std::placeholders::_1;

DataPaths::DataPaths (Bus* bus, CpuData* cpuData, MMU* mmu)
    :
    bus_ {bus},
    cpuData_ {cpuData},
    mmu_ {mmu}
{
    bus_->RESET ().subscribe (bind (&DataPaths::ResetReceiver, this, _1));
}

void DataPaths::reset ()
{
    if (mmu_ != nullptr)
        mmu_->reset ();
}

CondData<u16> DataPaths::fetchWord (VirtualAddress address,
    PSW::Mode memMgmtMode)
{
    CondData<u16> data;

    if (mmu_ != nullptr)
        data = mmu_->fetchWord (address, memMgmtMode);
    else
        data = busRead (address);

    if (console_ != nullptr)
        console_->display (address, data);

    return data;
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
    if (console_ != nullptr)
        console_->display (address, value);

    if (mmu_ != nullptr)
        return mmu_->putWord (address, value, memMgmtMode);
    else
        return busWrite (address, value);
}

bool DataPaths::putByte (VirtualAddress address, u8 value,
    PSW::Mode memMgmtMode)
{
    if (mmu_ != nullptr)
        return mmu_->putByte (address, value, memMgmtMode);
    else
        return busWriteByte (address, value);
}

void DataPaths::setVirtualPC (u16 value)
{
    if (mmu_ != nullptr)
        mmu_->setVirtualPC (value);
}

CondData<u16> DataPaths::readWithoutTrap (u16 address)
{
    return bus_->read (address);
}

void DataPaths::ResetReceiver (bool signalValue)
{
    if (signalValue)
        reset ();
}

CondData<u16> DataPaths::busRead (VirtualAddress address)
{
    CondData<u16> value = bus_->read (address);
    if (!value.hasValue ())
    {
        trace.bus (BusRecordType::ReadFail, address, 0);
        cpuData_->setTrap (CpuData::TrapType::BusError);
        return {};
    }
    return value;
}

bool DataPaths::busWrite (VirtualAddress address, u16 value)
{
    if (!bus_->writeWord (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        cpuData_->setTrap (CpuData::TrapType::BusError);
        return false;
    }
    return true;
}

bool DataPaths::busWriteByte (VirtualAddress address, u16 value)
{
    if (!bus_->writeByte (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        cpuData_->setTrap (CpuData::TrapType::BusError);
        return false;
    }
    return true;
}

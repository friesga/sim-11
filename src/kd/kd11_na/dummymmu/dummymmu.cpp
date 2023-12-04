#include "dummymmu.h"
#include "trace/trace.h"

DummyMMU::DummyMMU (Qbus* bus, CpuData* cpuData)
    :
    bus_ {bus},
    cpuData_ {cpuData}
{}

void DummyMMU::reset ()
{}

CondData<u16> DummyMMU::fetchWord (u16 address)
{
    // return bus_->read (address);
    CondData<u16> value = bus_->read (address);
    if (!value.hasValue ())
    {
        trace.bus (BusRecordType::ReadFail, address, 0);
        cpuData_->setTrap (CpuData::TrapCondition::BusError);
        return {};
    }
    return value;
}

// Fetch the byte at the given word or byte address
// 
// The validity of the fetched word has to be checked before the shift-
// and and-operators can be applied to the word!
CondData<u8> DummyMMU::fetchByte (u16 address)
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

bool DummyMMU::putWord (u16 address, u16 value)
{
    if (!bus_->writeWord (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        cpuData_->setTrap (CpuData::TrapCondition::BusError);
        return false;
    }
    return true;
}

bool DummyMMU::putByte (u16 address, u8 value)
{
    if (!bus_->writeByte (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        cpuData_->setTrap (CpuData::TrapCondition::BusError);
        return false;
    }
    return true;
}

// Pop a word from the processor stack returning true if this succeeds
// or false when a bus error occurs.
bool DummyMMU::popWord (u16 *destination)
{
    CondData<u16> tmpValue = fetchWord (cpuData_->registers ()[6]);
    *destination = tmpValue.valueOr (0);
    cpuData_->registers ()[6] += 2;
    if (!tmpValue.hasValue ())
        return false;
    return true;
}

// Push the given value on the processor stack
bool DummyMMU::pushWord (u16 value)
{
    cpuData_->registers ()[6] -= 2;
    return putWord (cpuData_->registers ()[6], value);
}

CondData<u16> DummyMMU::mappedRead (u16 address)
{
    return bus_->read (address);
}

bool DummyMMU::mappedWriteWord (u16 address, u16 value)
{
    return bus_->writeWord (address, value);
}

bool DummyMMU::mappedWriteByte (u16 address, u8 value)
{
    return bus_->writeByte (address, value);
}

void DummyMMU::setSR2 (u16 value)
{}

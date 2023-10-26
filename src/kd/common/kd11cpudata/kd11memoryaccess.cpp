#include "kd11cpudata.h"
#include "trace/trace.h"

CondData<u16> KD11CpuData::fetchWord (u16 address)
{
    // return bus_->read (address);
    CondData<u16> value = bus_->read (address);
    if (!value.hasValue ())
    {
        trace.bus (BusRecordType::ReadFail, address, 0);
        setTrap (&busError);
        return {};
    }
    return value;
}

// Fetch the byte at the given word or byte address
// 
// The validity of the fetched word has to be checked before the shift-
// and and-operators can be applied to the word!
CondData<u8> KD11CpuData::fetchByte (u16 address)
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

bool KD11CpuData::putWord (u16 address, u16 value)
{
    if (!bus_->writeWord (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        setTrap (&busError);
        return false;
    }
    return true;
}

bool KD11CpuData::putByte (u16 address, u8 value)
{
    if (!bus_->writeByte (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        setTrap (&busError);
        return false;
    }
    return true;
}

// Pop a word from the processor stack returning true if this succeeds
// or false when a bus error occurs.
bool KD11CpuData::popWord (u16 *destination)
{
    CondData<u16> tmpValue = fetchWord (registers ()[6]);
    *destination = tmpValue.valueOr (0);
    registers ()[6] += 2;
    if (!tmpValue.hasValue ())
        return false;
    return true;
}

// Push the given value on the processor stack
bool KD11CpuData::pushWord (u16 value)
{
    registers ()[6] -= 2;
    return putWord (registers ()[6], value);
}

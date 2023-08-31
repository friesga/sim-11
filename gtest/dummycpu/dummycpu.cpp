#include "dummycpu.h"

#include <memory>

using std::make_unique;

DummyCpu::DummyCpu ()
{
	memory_ = make_unique<u16[]> (memorySize_);
}

CondData<u16> DummyCpu::fetchWord (u16 address)
{
    if (address < memorySize_)
	    return CondData<u16> {memory_[address]};
    else
        return CondData<u16> {};
}

CondData<u8> DummyCpu::fetchByte (u16 address)
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

bool DummyCpu::putWord (u16 address, u16 value)
{
    if (address < memorySize_)
    {
	    memory_[address] = value;
	    return true;
    }
    else
        return false;
}

bool DummyCpu::putByte (u16 address, u8 value)
{
	CondData<u16> tmp = fetchWord (address);
	if(address & 1)
		tmp = (tmp & 0x00FF) | (value << 8);
	else
		tmp = (tmp & 0xFF00) | value;

	return putWord (address, tmp);

	return false;
}
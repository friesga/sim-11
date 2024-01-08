#include "dummymmu.h"

#include <memory>

using std::make_unique;

DummyMMU::DummyMMU ()
{
	memory_ = make_unique<u16[]> (memorySize_);
}

CondData<u16> DummyMMU::fetchWord (VirtualAddress address, PSW::Mode memMgmtMode)
{
    if (address < memorySize_)
	    return CondData<u16> {memory_[address]};
    else
        return CondData<u16> {};
}

CondData<u8> DummyMMU::fetchByte (VirtualAddress address, PSW::Mode memMgmtMode)
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

bool DummyMMU::putWord (VirtualAddress address, u16 value, PSW::Mode memMgmtMode)
{
    if (address < memorySize_)
    {
	    memory_[address] = value;
	    return true;
    }
    else
        return false;
}

bool DummyMMU::putByte (VirtualAddress address, u8 value, PSW::Mode memMgmtMode)
{
	CondData<u16> tmp = fetchWord (address);
	if(address & 1)
		tmp = (tmp & 0x00FF) | (value << 8);
	else
		tmp = (tmp & 0xFF00) | value;

	return putWord (address, tmp);

	return false;
}
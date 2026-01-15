#include "ka11cpudata.h"

CondData<u16> KA11CpuData::read (BusAddress address)
{

}

StatusCode KA11CpuData::writeWord (BusAddress address, u16 value)
{
    return StatusCode::NonExistingMemory;
}

bool KA11CpuData::responsible (BusAddress address)
{
    return ((address.registerAddress () & 0177776) == PSWAddress) ? true : false;
}

void KA11CpuData::reset ()
{ }
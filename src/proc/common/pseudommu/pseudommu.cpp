#include "pseudommu.h"
#include "trace/trace.h"

PseudoMMU::PseudoMMU (Bus* bus, CpuData* cpuData)
    :
    bus_ {bus},
    cpuData_ {cpuData}
{}

void PseudoMMU::reset ()
{}

// The PSW::Mode is a default parameter which is not used in the PseudoMMU.
CondData<u16> PseudoMMU::fetchWord (VirtualAddress address, PSW::Mode mode)
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


// The PSW::Mode is a default parameter which is not used in the PseudoMMU.
bool PseudoMMU::putWord (VirtualAddress address, u16 value, PSW::Mode memMgmtMode)
{
    if (!bus_->writeWord (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        cpuData_->setTrap (CpuData::TrapType::BusError);
        return false;
    }
    return true;
}

// The PSW::Mode is a default parameter which is not used in the PseudoMMU.
bool PseudoMMU::putByte (VirtualAddress address, u8 value, PSW::Mode memMgmtMode)
{
    if (!bus_->writeByte (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        cpuData_->setTrap (CpuData::TrapType::BusError);
        return false;
    }
    return true;
}

// Pop a word from the processor stack returning true if this succeeds
// or false when a bus error occurs.
bool PseudoMMU::popWord (u16 *destination)
{
    CondData<u16> tmpValue = fetchWord (cpuData_->registers ()[6]);
    *destination = tmpValue.valueOr (0);
    cpuData_->registers ()[6] += 2;
    if (!tmpValue.hasValue ())
        return false;
    return true;
}

// Push the given value on the processor stack
bool PseudoMMU::pushWord (u16 value)
{
    cpuData_->registers ()[6] -= 2;
    return putWord (cpuData_->registers ()[6], value);
}

CondData<u16> PseudoMMU::readWithoutTrap (u16 address)
{
    return bus_->read (address);
}

void PseudoMMU::setVirtualPC (u16 value)
{}

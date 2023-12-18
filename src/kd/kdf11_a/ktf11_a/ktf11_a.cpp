#include "ktf11_a.h"
#include "kd/include/psw.h"

KTF11_A::KTF11_A (Qbus* bus, CpuData* cpuData)
    :
    bus_ {bus},
    cpuData_ {cpuData}
{}
CondData<u16> KTF11_A::fetchWord (u16 address)
{
    // return bus_->read (address);
    CondData<u16> value = mappedRead (address);
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
CondData<u8> KTF11_A::fetchByte (u16 address)
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

bool KTF11_A::putWord (u16 address, u16 value)
{
    if (!mappedWriteWord (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        cpuData_->setTrap (CpuData::TrapCondition::BusError);
        return false;
    }
    trace.MmuApr (activePageRegisterSet_[KERNEL_MODE]);
    return true;
}

bool KTF11_A::putByte (u16 address, u8 value)
{
    if (!mappedWriteByte (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        cpuData_->setTrap (CpuData::TrapCondition::BusError);
        return false;
    }
    return true;
}

// Pop a word from the processor stack returning true if this succeeds
// or false when a bus error occurs.
bool KTF11_A::popWord (u16 *destination)
{
    CondData<u16> tmpValue = fetchWord (cpuData_->registers ()[6]);
    *destination = tmpValue.valueOr (0);
    cpuData_->registers ()[6] += 2;
    if (!tmpValue.hasValue ())
        return false;
    return true;
}

// Push the given value on the processor stack
bool KTF11_A::pushWord (u16 value)
{
    cpuData_->registers ()[6] -= 2;
    return putWord (cpuData_->registers ()[6], value);
}

// Return the word at the given virtual address using the MMU mapping
CondData<u16> KTF11_A::mappedRead (u16 address)
{
    if (sr0_.managementEnabled ())
        return bus_->read (physicalAddress (address));

    return bus_->read (address);
}

// Write the word at the given virtual address using the MMU mapping.
// 
// First register the write access to the page and then write the word
// to prevent that a write to a PDR clearing the W-bit will immediately
// lead to setting the W-bit again. This satifies diagnostic JKDAD1 test 26.
//
// JKDAD1 test 30 describes and tests two W-bit special cases:
// 1. The KPDR7 W-bit should not be set when writing to Status Register 0,
// 2. The W-bit is still set if "the DATO is aborted due to a time-out 
//    error", i.e. if the write fails. 
//
bool KTF11_A::mappedWriteWord (u16 address, u16 value)
{
    if (sr0_.managementEnabled ())
    {
        ActivePageRegister* apr = activePageRegister (address);
        if (address != statusRegister0)
            apr->pageDescripterRegister_.setWriteAccess ();
        return bus_->writeWord (physicalAddress (address, apr), value);
    }

    return bus_->writeWord (address, value);
}

// Write the byte at the given virtual address using the MMU mapping
bool KTF11_A::mappedWriteByte (u16 address, u8 value)
{
    if (sr0_.managementEnabled ())
    {
        ActivePageRegister* apr = activePageRegister (address);
        if ((address & 0177776) != statusRegister0)
            apr->pageDescripterRegister_.setWriteAccess ();
        return bus_->writeByte (physicalAddress (address), value);
    }

    return bus_->writeByte (address, value);
}

// Return the Active Page Field (APF) from the given virtual address
constexpr u16 KTF11_A::activePageField (u16 address)
{
    return (address & APFMask) >> APFIndex;
}

// Return the block number from the given virtual address
constexpr u16 KTF11_A::blockNumber (u16 address)
{
    return (address & blockNrMask) >> blockNrIndex;
}

// Return the displacement in block from the given virtual address
constexpr u16 KTF11_A::displacementInBlock (u16 address)
{
    return address & DIBMask;
}

// Return the current memory management mode
constexpr u16 KTF11_A::memoryManagementMode ()
{
    return (cpuData_->psw () & PSW_MEM_MGMT_MODE) >> PSW_MEM_MGMT_MODE_P;
}

// Return a pointer to the active (i.e. kernel or user) page register for the
// given virtual address. The page register contains the PAR and PDR.
ActivePageRegister *KTF11_A::activePageRegister (u16 address)
{
    u16 apf = activePageField (address);
    return &activePageRegisterSet_[memoryManagementMode ()].activePageRegister_[apf];
}

// Return the 22-bit physical address for the given 16-bit virtual address
//
// The logical sequence involved in forming a physical address is as follows. 
// 
// 1. Select a set of active page registers. (Selection depends on the current
//    mode specified by PS<15:14>.) 
// 2. The active page field of the virtual address is used to select an active
//    page register (APR0-APR 7). 
// 3. The page address field of the selected APR contains the starting address
//    of the currently active page as a block number in physical memory. 
// 4. The block number from the virtual address is added to the block number
//    from the page address field to yield the number of the block in physical
//    memory that will contain the physical address being constructed. 
// 5. The displacement in blocks from the displacement field of the virtual
//    address is joined to the physical block number to yield a 22-bit physical
//    address. 
// 
// Source: EK-KDF11-UG-PR2
//
u32 KTF11_A::physicalAddress (u16 address)
{
    return physicalAddress (address, activePageRegister (address));
}

// Return the 22-bit physical address for the given 16-bit virtual address
// using the already determined page address field.
u32 KTF11_A::physicalAddress (u16 address, ActivePageRegister* apr)
{
    u32 physicalBlockNr = apr->pageAddressRegister_ + blockNumber (address);
    return (physicalBlockNr << 6) | displacementInBlock (address);
}

// The content of SR2 (the virtual program counter) is frozen whenever one of
// the abort flags (SR0<15:13>) is set.
void KTF11_A::setVirtualPC (u16 value)
{
    if (!sr0_.accessAborted ())
        sr2_ = value;
}

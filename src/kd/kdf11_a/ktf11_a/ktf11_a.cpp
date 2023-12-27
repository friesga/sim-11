#include "ktf11_a.h"
#include "kd/include/psw.h"

KTF11_A::KTF11_A (Qbus* bus, CpuData* cpuData)
    :
    bus_ {bus},
    cpuData_ {cpuData}
{}

CondData<u16> KTF11_A::fetchWord (u16 address, PSW::Mode memMgmtMode)
{
    u16 mode = (memMgmtMode == PSW::Mode::Default) ?
        memoryManagementMode () :
        static_cast<u16> (memMgmtMode);

    return (sr0_.managementEnabled ()) ? 
        mappedRead (address, mode) :
        readPhysical (address);
}

// Fetch the byte at the given word or byte address
// 
// The validity of the fetched word has to be checked before the shift-
// and and-operators can be applied to the word!
CondData<u8> KTF11_A::fetchByte (u16 address, PSW::Mode memMgmtMode)
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

bool KTF11_A::putWord (u16 address, u16 value, PSW::Mode memMgmtMode)
{
    u16 mode = (memMgmtMode == PSW::Mode::Default) ?
        memoryManagementMode () :
        static_cast<u16> (memMgmtMode);

    return (sr0_.managementEnabled ()) ? 
        mappedWriteWord (address, value, mode) :
        writePhysicalWord (address, value);
}

bool KTF11_A::putByte (u16 address, u8 value, PSW::Mode memMgmtMode)
{
    u16 mode = (memMgmtMode == PSW::Mode::Default) ?
        memoryManagementMode () :
        static_cast<u16> (memMgmtMode);

    return (sr0_.managementEnabled ()) ? 
        mappedWriteByte (address, value, mode) :
        writePhysicalByte (address, value);
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
CondData<u16> KTF11_A::mappedRead (u16 address, u16 mode)
{
    ActivePageRegister* apr = activePageRegister (address, mode);

    if (!pageResident (apr->pageDescripterRegister_))
        return abortAccess (SR0::AbortReason::NonResident, address);

    if (!withinPage (blockNumber (address), apr->pageDescripterRegister_))
        return abortAccess (SR0::AbortReason::PageLengthError, address);
    
    return readPhysical (physicalAddress (address, apr));
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
// The abort flags are in priority order in that flags to the right are less
// significant and should be ignored when a more significant flag is asserted.
// For example, a nonresident abort service routine would ignore the
// page-length bit (14) and read-only access violation bit (13).
// A page-length abort service routine would ignore the read-only access
// violation bit. (EK-KDF11-UG-PR2)
//
bool KTF11_A::mappedWriteWord (u16 address, u16 value, u16 mode)
{
    ActivePageRegister* apr = activePageRegister (address, mode);
    if (address != statusRegister0)
            apr->pageDescripterRegister_.setWriteAccess ();

    if (!pageResident (apr->pageDescripterRegister_))
        return abortAccess (SR0::AbortReason::NonResident, address);

    if (!withinPage (blockNumber (address), apr->pageDescripterRegister_))
        return abortAccess (SR0::AbortReason::PageLengthError, address);

    if (!writeAllowed (apr->pageDescripterRegister_))
        return abortAccess (SR0::AbortReason::ReadOnlyAccessViolation, address);

    return writePhysicalWord (physicalAddress (address, apr), value);
}

// Write the byte at the given virtual address using the MMU mapping
bool KTF11_A::mappedWriteByte (u16 address, u8 value, u16 mode)
{
    ActivePageRegister* apr = activePageRegister (address, mode);
    if ((address & 0177776) != statusRegister0)
        apr->pageDescripterRegister_.setWriteAccess ();

    if (!pageResident (apr->pageDescripterRegister_))
        return abortAccess (SR0::AbortReason::NonResident, address);

    if (!withinPage (blockNumber (address), apr->pageDescripterRegister_))
        return abortAccess (SR0::AbortReason::PageLengthError, address);

    if (!writeAllowed (apr->pageDescripterRegister_))
        return abortAccess (SR0::AbortReason::ReadOnlyAccessViolation, address);

    return writePhysicalByte (physicalAddress (address, apr), value);
}


// Return the word at the given virtual address using the MMU mapping
// without setting a trap in case of a buserror. This function is a
// special case, used by CpuControl's step() function to read instruction
// words.
CondData<u16> KTF11_A::readWithoutTrap (u16 address)
{
    return (sr0_.managementEnabled ()) ? 
        bus_->read (physicalAddress (address)) :
        bus_->read (address);
}

// Return the word at the given physical address, generating a bus error trap
// in case the read fails.
CondData<u16> KTF11_A::readPhysical (u16 address)
{
    CondData<u16> value = bus_->read (address);
    if (!value.hasValue ())
    {
        trace.bus (BusRecordType::ReadFail, address, 0);
        cpuData_->setTrap (CpuData::TrapCondition::BusError);
    }

    return value;
}

// Write the given value to the given physical address, generating a bus
// error trap in case the write fails.
bool KTF11_A::writePhysicalWord (u16 address, u16 value)
{
    if (!bus_->writeWord (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        cpuData_->setTrap (CpuData::TrapCondition::BusError);
        return false;
    }

    return true;
}

bool KTF11_A::writePhysicalByte (u16 address, u16 value)
{
    if (!bus_->writeByte (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        cpuData_->setTrap (CpuData::TrapCondition::BusError);
        return false;
    }

    return true;
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
    return static_cast<u16> (cpuData_->psw ().currentMode ());
}

// Return a pointer to the (kernel or user) page register for the
// given virtual address, using the given mode. The page register contains
// the PAR and PDR.
ActivePageRegister *KTF11_A::activePageRegister (u16 address, u16 mode)
{
    u16 apf = activePageField (address);
    return &activePageRegisterSet_[mode].activePageRegister_[apf];
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
    return physicalAddress (address, 
        activePageRegister (address, memoryManagementMode ()));
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

// This function checks whether or not the page with the given Page
// Descriptor Register is resident. The KDF11-A User Guide defines the
// value 2 (key 4) as "Unused" but diagnostic JKDAD1 test 30 shows that
// accesses to a page with that value have to be aborted.
bool KTF11_A::pageResident (PDR const & pdr)
{
    PDR::AccessControlKey key = pdr.accessControlKey ();
    return key != PDR::AccessControlKey::NonResident &&
        key != PDR::AccessControlKey::Unused;
}

bool KTF11_A::writeAllowed (PDR const & pdr)
{
    return pdr.accessControlKey () == PDR::AccessControlKey::ReadWrite;
}

bool KTF11_A::abortAccess (SR0::AbortReason reason, u16 address)
{
    sr0_.setAbortCondition (reason, 
        static_cast<u16> (cpuData_->psw ().currentMode ()),
        activePageField (address));
    cpuData_->setTrap (CpuData::TrapCondition::MemoryManagementTrap);
    return false;
}

// This function checks if the given block number is within the page 
// described by the given page descriptor register.
//
// [In case the page Expansion Direction is upward and] VBA<12:6> [i.e. the
// block number] is less than or equal to PDR:14:8> [i.e. the page length
// field] a page length abort should occur and be reported by SR0 and SR2.
// [In case the page Expansion Direction is downward] and VBA<12:6> is less
// than PDR<14:8> a page length abort should occur...] (JKDAD1 TEST 33 and
// TEST 34)
bool KTF11_A::withinPage (u16 blockNumber, PDR const & pdr)
{
    return (pdr.expansionDirection () == PDR::ExpansionDirection::Upward) ?
        blockNumber <= pdr.pageLength () :
        blockNumber >= pdr.pageLength ();
}
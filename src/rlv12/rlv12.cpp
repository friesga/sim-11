#include "rlv12.h"
#include "types.h"

#include <thread>
#include <chrono>

// RLV12 constructor
// Set name and default base address and vector. Set the controller
// ready to accept a command.
// The base class protected data members cannot be accessed in an
// initializer list
// ToDo: Remove default RLV12 constructor
RLV12::RLV12 ()
    :
    rlcs {0},
    rlba {0},
    rlda {0},
    rlbae {0},
    rlxb_ {nullptr},
    rlType_ {RlConfig::RLType::RLV12},
    _22bit_ {false},
    wordCounter_ {0},
    fifoIndex_ {0}
{
    name_ = "RL";
    baseAddress_ = RLV_BaseAddress;
    vector_ = RLV_Vector;

    // Allocate the transfer buffer, initializing to zero
    rlxb_ = new (std::nothrow) u16[maxTransfer]();

    if (rlxb_ == nullptr)
        throw ("Allocating memory for transfer buffer failed");

    // Start the command processor
    cmdProcessor = std::make_unique<CmdProcessor> (this);
}

RLV12::RLV12 (RlConfig *rlConfig)
    :
    rlcs {0},
    rlba {0},
    rlda {0},
    rlbae {0},
    rlxb_ {nullptr},
    rlType_ {rlConfig->rlType},
    _22bit_ {rlConfig->_22bit},
    wordCounter_ {0},
    fifoIndex_ {0}
{
    name_ = "RL";
    baseAddress_ = (rlConfig->address > 0) ? 
        rlConfig->address : RLV_BaseAddress;
    vector_ = (rlConfig->vector > 0) ? rlConfig->vector : RLV_Vector;

    // Allocate the transfer buffer, initializing to zero
    rlxb_ = new (std::nothrow) u16[maxTransfer]();

    if (rlxb_ == nullptr)
        throw ("Allocating memory for transfer buffer failed");

    // Start the command processor
    cmdProcessor = std::make_unique<CmdProcessor> (this);
}

// Destructor to deallocate transfer buffer
// ToDo: Delete copy constructor and copy assignment operator
RLV12::~RLV12 ()
{
    if (rlxb_ != nullptr)
        delete [] rlxb_;
}

//
// RLV12 device reset
//
// The proper bits of the CSR have to be reset. Those bits are bits 1-6,
// 8-13 and 15. Bit 15 has to be cleared only if bit 14 (Drive Error) is not
// set. Bit 0 (Drive Ready) is a don't care. Source: ZRLGE0 test 14.
// 
// Note that the RL11 does NOT recalibrate its drives on RESET.
// Source: simh comment.
void RLV12::reset ()
{
    constexpr u16 properBits = 037576;

    // Reset CRS
    rlcs &= ~properBits;
    if (!(rlcs & CSR_DriveError))
        rlcs &= ~CSR_CompositeError;

    rlba = 0;
    rlda = 0;
    rlbae = 0;
    
    // ToDo: Clear interrupt request
    // CLR_INT(RL);

    // for (RL01_2 unit : units_)
    // {
        // ToDo: Cancel outstanding timer for this unit
        // sim_cancel(uptr);

        // ToDo: Clear unit error condition
        // N.B. STAT == u4
        // uptr->STAT &= ~RLDS_ERR;
    // }
}

// An RLV11 controller or an RLV12 with the 22-bit option disabled has no
// Bus Address Extension register. This function will return true for
// an access to that register; following reads and writes to that register
// however will return a non-existing memory error.
bool RLV12::responsible (u16 address)
{
    return address >= baseAddress_ && address <= (baseAddress_ + BAE) ?
        true : false;
}

// Update the BA register and the CSR BA16 and BA17 bits from the given
// memory address. In case of an RLV12 (with enabled 22-bit addressing)
// also update the BAE register. For systems without a BAE keep the contents
// of that register zero, so it can be used to calculate a memory address
// for 16-, 18- and 22-bit systems.
void RLV12::memAddrToRegs (u32 memoryAddress)
{
    // Load the lower 16 bits in the BAR, the upper two bits in the RLCS
    // and the upper 6 bits in the BAE
    u16 upper6Bits = memoryAddress >> 16;
    rlba = memoryAddress & BAR_Bits;
    // rlcs = (rlcs & ~RLCS_MEX) | ((upper6Bits & RLCS_M_MEX) << RLCS_V_MEX);
    rlcs = (rlcs & ~CSR_AddressExtField) | 
        ((upper6Bits & CSR_AddressExtBits) << CSR_AddressExtPosition);

    if (rlType_ == RlConfig::RLType::RLV12 && _22bit_)
        rlbae = upper6Bits & BAE_Bits; 
}

// Get a 16-, 18- or 22-bit address from the BA, CSR and (in case of 22-bit
// systems) BAE registers. For 16- and 18-bit systems the BA16 and BA17 bits
// are used, for 22-bit systems the BAE register is used.
u32 RLV12::memAddrFromRegs ()
{
    if (!(rlType_ == RlConfig::RLType::RLV12 && _22bit_))
        return (getBA16BA17 (rlcs) << 16) | rlba;
    else
        return (rlbae << 16) | rlba;
}

// Update the BAE register bits 00 and 01 from the contents of the CSR BA16
// and BA17 bits
void RLV12::updateBAE ()
{
    if (rlType_ == RlConfig::RLType::RLV12 && _22bit_)
        rlbae = (rlbae & ~CSR_AddressExtBits) | 
            ((rlcs >> CSR_AddressExtPosition) & CSR_AddressExtBits);
}

// Get the BA16 and BA17 bits from the given csr
constexpr u16 RLV12::getBA16BA17 (u16 csr)
{
    return (csr >> CSR_AddressExtPosition) & CSR_AddressExtBits;
}
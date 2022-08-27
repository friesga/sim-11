#include "rlv12.h"

#include "types.h"

// RLV12 constructor
// Set name and default base address and vector
// The base class protected data members cannot be accessed in an
// initializer list
// ToDo: Remove default RLV12 constructor
RLV12::RLV12 ()
    :
    rlcs {0},
    rlba {0},
    rlda {0},
    rlmpr {0},
    rlbae {0},
    rlxb_ {nullptr},
    flags_ {0}
{
    name_ = "RL";
    baseAddress_ = IOBA_RL;
    vector_ = VEC_RL;

    // Allocate the transfer buffer, initializing to zero
    rlxb_ = new (std::nothrow) u16[RL_MAXFR]();

    if (rlxb_ == nullptr)
        throw ("Allocating memory for transfer buffer failed");

    // Reset the controller
    reset ();
}

RLV12::RLV12 (u32 baseAddress, u32 vector, bool RLV11, size_t numUnits)
    :
    rlcs {0},
    rlba {0},
    rlda {0},
    rlmpr {0},
    rlbae {0},
    rlxb_ {nullptr},
    flags_ {0}
{
    name_ = "RL";
    baseAddress_ = (baseAddress > 0) ? baseAddress : IOBA_RL;
    vector_ = (vector > 0) ? vector : VEC_RL;

    if (RLV11)
        flags_ |= DEV_RLV11;

    // Allocate the transfer buffer, initializing to zero
    rlxb_ = new (std::nothrow) u16[RL_MAXFR]();

    if (rlxb_ == nullptr)
        throw ("Allocating memory for transfer buffer failed");

    // Reset the controller
    reset ();
}

// Destructor to deallocate transfer buffer
RLV12::~RLV12 ()
{
    if (rlxb_ != nullptr)
        delete [] rlxb_;
}

//
// RLV12 device reset
//
// Note that the RL11 does NOT recalibrate its drives on RESET
//
void RLV12::reset ()
{
    rlcs = CSR_DONE;
    rlba = 0;
    rlda = 0;
    rlmpr = 0;
    rlbae = 0;
    
    // ToDo: Clear interrupt request
    // CLR_INT(RL);

    for (RL01_2 unit : units_)
    {
        // ToDo: Cancel outstanding timer for this unit
        // sim_cancel(uptr);

        // ToDo: Clear unit error condition
        // N.B. STAT == u4
        // uptr->STAT &= ~RLDS_ERR;
    }
}

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
    rlba = memoryAddress & RLBA_IMP;
    rlcs = (rlcs & ~RLCS_MEX) | ((upper6Bits & RLCS_M_MEX) << RLCS_V_MEX);

    if (!(flags_ & DEV_RLV11))
        rlbae = upper6Bits & RLBAE_IMP; 
}

// Get a 16-, 18- or 22-bit address from the BA, CSR and (in case of 22-bit
// systems) BAE registers. For 16- and 18-bit systems the BA16 and BA17 bits
// are used, for 22-bit systems the BAE register is used.
u32 RLV12::memAddrFromRegs ()
{
    if (flags_ & DEV_RLV11)
        return (getBA16BA17 (rlcs) << (16 - RLCS_V_MEX)) | rlba;
    else
        return (rlbae << 16) | rlba;
}

// Update the BAE register bits 00 and 01 from the contents of the CSR BA16
// and BA17 bits
void RLV12::updateBAE ()
{
    if (!(flags_ & DEV_RLV11))
       rlbae = (rlbae & ~RLCS_M_MEX) | ((rlcs >> RLCS_V_MEX) & RLCS_M_MEX);
}

// Get the BA16 and BA17 bits from the given csr
constexpr u16 RLV12::getBA16BA17 (u16 csr)
{
    return (csr >> RLCS_V_MEX) & RLCS_M_MEX;
}
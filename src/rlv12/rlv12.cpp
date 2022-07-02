#include "rlv12.h"

#include "types.h"

// RLV12 constructor
// Set name and default base address and vector
// The base class protected data members cannot be accessed in an
// initializer list
RLV12::RLV12 ()
    :
    rlxb_ {nullptr}
{
    name_ = "RL";
    baseAddress_ = IOBA_RL;
    vector_ = VEC_RL;
}

// Destructor to deallocate transfer buffer
RLV12::~RLV12 ()
{
    if (rlxb_ != nullptr)
        delete rlxb_;
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

    if (rlxb_ == nullptr)
        // Allocate the transfer buffer, initializing to zero
        rlxb_ = new (std::nothrow) u16[RL_MAXFR]();

    if (rlxb_ == nullptr)
        throw ("Allocating memory for transfer buffer failed");
}
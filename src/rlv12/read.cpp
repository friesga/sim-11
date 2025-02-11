#include "rlv12.h"

/*
 *  I/O dispatch routines, I/O addresses 17774400 - 17774411
 *
 *  17774400 RLCS    read/write
 *  17774402 RLBA    read/write
 *  17774404 RLDA    read/write
 *  17774406 RLMP    read/write
 *  17774410 RLBAE   read/write
 */
CondData<u16> RLV12::read (BusAddress busAddress)
{
    // Decode registerAddress<3:1>
    switch (busAddress.registerAddress () & 016)
    {
        case CSR:
        {
            // Control/Status Register
            // Add the Drive Ready and Drive Error bits to the CSR be 
            // returned. This avoids having to change the actual CSR and
            // having to lock the controller mutex.
            u16 data = rlcsPlusDriveStatus (units_[RLV12const::getDrive (csr_)]);
            trace.rlv12Registers ("read CSR", data, bar_, dar_,
                dataBuffer_[0], bae_);
            return {data};
        }

        case BAR:
            // Bus Address register
            return {static_cast<u16> (bar_ & RLV12const::BAR_Mask)};

        case DAR:
            // Disk Address register
            return {dar_};

        case MPR: 
            // Multipurpose register
            // *data = rlmpr;
            // rlmpr = rlmpr1;         // ripple data
            // rlmpr1 = rlmpr2;
            return {dataBuffer_[fifoIndex_++]};
            break;

        case BAE:
            // Bus Address Extension register
            // Not present in RL11/RLV11 and not enabled on a RLV12 with the
            // 22-bit option disabled
            if (!(rlType_ == RLV12const::RLType::RLV12 && _22bit_))
               return StatusCode::NonExistingMemory;

            return {static_cast<u16> (bae_ & RLV12const::BAE_Mask)};
            break;

        default:
            return (StatusCode::NonExistingMemory);
    }

    return StatusCode::Success;
}
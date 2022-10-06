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
StatusCode RLV12::read (u16 registerAddress, u16* data)
{
    // Guard against controller register updates from the command processor
	std::unique_lock<std::mutex> lock {controllerMutex_};

    // Decode registerAddress<3:1>
    switch (registerAddress & 016)
    {
        case CSR:
            // Control/Status Register
            TRACERLV12Registers (&trc, "read CSR", rlcs, rlba, rlda, rlmpr, rlbae);
            *data = rlcs;
            break;

        case BAR:
            // Bus Address register
            *data = rlba & RLBA_IMP;
            break;

        case DAR:
            // Disk Address register
            *data = rlda;
            break;

        case MPR: 
            // Multipurpose register
            *data = rlmpr;
            rlmpr = rlmpr1;         // ripple data
            rlmpr1 = rlmpr2;
            break;

        case BAE:
            // Bus Address Extension register
            // Not present in RL11/RLV11 and not enabled on a RLV12 with the
            // 22-bit option disabled
            if (!(rlType_ == RlConfig::RLType::RLV12 && _22bit_))
               return StatusCode::NonExistingMemory;

            *data = rlbae & RLBAE_IMP;
            break;

        default:
            return (StatusCode::NonExistingMemory);
    }

    return StatusCode::OK;
}
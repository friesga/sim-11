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

    // Decode registerAddress<3:1>
    switch (registerAddress & 016)
    {                               
        case CSR:
        {
            // Control/Status Register
            // ToDo: Setting BA16/BA17 bits at this point should be 
            // superfluous when the CSR is correctly maintained.
            // rlcs = (rlcs & ~RLCS_MEX) | ((rlbae & RLCS_M_MEX) << RLCS_V_MEX);

            /*
            The DRDY signal is sent by the selected drive to indicate that it
            is ready to read or write or seek.  It is sent when the heads are
            not moving and are locked onto a cylinder.  This is continuously
            monitored by the drive and controller. [EK-0RL11-TD-001, p. 3-8]
            Use the DS bits to determine if the drive has any outstanding I/O
            operations and set DRDY as appropriate.

            This seems to imply that only a Seek operation (not Read/Write)
            causes ready to be false.
            */

            RL01_2 &unit = units_[GET_DRIVE(rlcs)];

            if (unit.rlStatus_ & RlStatus::UNIT_OFFL || 
                unit.driveStatus_ & RLDS_VCK)
            {
                rlcs |= RLCS_DRE;
                rlcs &= ~RLCS_DRDY;
            }
            else if (timer.isRunning (&unit) ||     // see if drive ready 
                    unit.unitStatus_ & Status::UNIT_DIS ||
                    ((unit.driveStatus_ & RLDS_M_STATE) != RLDS_LOCK))
                rlcs &= ~RLCS_DRDY;
            else
                rlcs |= RLCS_DRDY;                              
            
            // Make sure the error summary bit properly reflects the 
            // sum of other errors.
            if (rlcs & RLCS_ALLERR)
                rlcs |= RLCS_ERR;
            *data = rlcs;

            TRACERLV12Registers (&trc, "read", rlcs, rlba, rlda, rlmpr, rlbae);
            break;
        }

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
            if (!(rlType_ == RLType::RLV12 && _22bit_))
               return StatusCode::NonExistingMemory;

            *data = rlbae & RLBAE_IMP;
            break;

        default:
            return (StatusCode::NonExistingMemory);
    }

    return StatusCode::OK;
}
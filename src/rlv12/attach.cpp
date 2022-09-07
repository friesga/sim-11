#include "rlv12.h"
#include "sim_fio/sim_fio.h"

StatusCode RL01_2::attach (std::string fileName, Bitmask<AttachFlags> flags)
{
    capacity_ = (rlStatus_ & RlStatus::UNIT_RL02) ? RL02_SIZE : RL01_SIZE;

    // Try to attach the specified file to this unit
    StatusCode result;
    if ((result = attach_unit (fileName, flags)) != StatusCode::OK)
        return result;

    // For compatibility with existing SIMH behavior, set the drive state
    // as if the load procedure had already executed.
    // ToDo: Check if this behaviour is wanted

    // Position at cylinder 0
    currentDiskAddress_ = 0;

    // New volume
    driveStatus_ = RLDS_HDO | RLDS_BHO | RLDS_VCK | RLDS_LOCK;

    // Set unit on-line
    rlStatus_ &= ~Bitmask(RlStatus::UNIT_OFFL);

    
    // Create a bad block table on a new disk image (if the 
    // image is not read-only)
    t_offset fileSize;
    if ((fileSize = sim_fsize (filePtr_)) == 0)
    {   
        // If read-only we're done
        if (unitStatus_ & Status::UNIT_RO)
            return StatusCode::OK;

        return createBadBlockTable (RL_NUMSC, RL_NUMWD);
    }

    // If auto-sizing is set, determine drive type on the file size
    if (rlStatus_ & RlStatus::UNIT_AUTO)
    {
        if (fileSize > (RL01_SIZE * sizeof (u16)))
        {
            rlStatus_ |= RlStatus::UNIT_RL02;
            capacity_ = RL02_SIZE;
        }
        else
        {
            rlStatus_ &= ~Bitmask(RlStatus::UNIT_RL02);
            capacity_ = RL01_SIZE;
        }
    }
    return StatusCode::OK;
}
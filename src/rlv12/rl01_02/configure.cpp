#include "../rlv12.h"
#include "sim_fio/sim_fio.h"

StatusCode RL01_2::configure (UnitConfig &unitConfig)
{
    RlUnitConfig &rlUnitConfig = static_cast<RlUnitConfig &> (unitConfig);

    // Set unit type and size from the given configuration. Note that if
    // the unit type is Auto the unit's capacity is determined after
    // attaching the file to the unit. The capacity is also needed for
    // creation of the bad block table, but the combination of the newFile
    // option and Auto unit type is excluded in the configuration data.
    // ToDo: RL01_2::configure() needs a rewrite.
    if (rlUnitConfig.rlUnitType == RlUnitConfig::RLUnitType::RL01)
    {
        capacity_ = RL01_SIZE;
        rlStatus_ &= ~Bitmask(RlStatus::UNIT_RL02);
    }
    else if (rlUnitConfig.rlUnitType == RlUnitConfig::RLUnitType::RL02)
    {
        capacity_ = RL02_SIZE;
        rlStatus_ |= Bitmask(RlStatus::UNIT_RL02);
    }
    else if (rlUnitConfig.rlUnitType == RlUnitConfig::RLUnitType::Auto)
    {
        rlStatus_ |= Bitmask(RlStatus::UNIT_AUTO);
    }

    if (rlUnitConfig.fileName.empty()) 
        return StatusCode::ArgumentError;
	
    Bitmask<AttachFlags> attachFlags {AttachFlags::Default};

	if (rlUnitConfig.readOnly)
		attachFlags |= AttachFlags::ReadOnly;
	if (rlUnitConfig.newFile) 
		attachFlags |= AttachFlags::NewFile;
	if (rlUnitConfig.overwrite)
		attachFlags |= AttachFlags::Overwrite;

    // Try to attach the specified file to this unit
    StatusCode result;
    if ((result = attach_unit (rlUnitConfig.fileName, attachFlags)) !=
        StatusCode::OK)
            return result;

    // Set the drive state as if the load procedure had already executed.

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

        // Create a bad block table on the last track of the device.
        // The position of the last track is also based on the unit's
        // capacity!
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
#include "../rlv12.h"
#include "fio/fio.h"

#include <variant>

using std::shared_ptr;
using std::get;

StatusCode RL01_02::configure (shared_ptr<RLUnitConfig> rlUnitConfig)
{
    // Set unit type and size from the given configuration. Note that if
    // the unit type is Auto the unit's capacity is determined after
    // attaching the file to the unit. The capacity is also needed for
    // creation of the bad block table, but the combination of the newFile
    // option and Auto unit type is excluded in the configuration data.
    // ToDo: RL01_02::configure() needs a rewrite.
    if (rlUnitConfig->rlUnitType == RLUnitConfig::RLUnitType::RL01)
    {
        capacity_ = RLV12const::RL01_WordsPerCartridge;
        rlStatus_ &= ~Bitmask(RlStatus::UNIT_RL02);
    }
    else if (rlUnitConfig->rlUnitType == RLUnitConfig::RLUnitType::RL02)
    {
        capacity_ = RLV12const::RL02_WordsPerCartridge;
        rlStatus_ |= Bitmask(RlStatus::UNIT_RL02);
        driveStatus_ |= RLV12const::MPR_GS_DriveType;
    }
    else if (rlUnitConfig->rlUnitType == RLUnitConfig::RLUnitType::Auto)
    {
        rlStatus_ |= Bitmask(RlStatus::UNIT_AUTO);
    }

    if (rlUnitConfig->fileName.empty()) 
        return StatusCode::ArgumentError;
	
    Bitmask<AttachFlags> attachFlags {AttachFlags::Default};

	if (rlUnitConfig->writeProtect)
		attachFlags |= AttachFlags::ReadOnly;
	if (rlUnitConfig->newFile) 
		attachFlags |= AttachFlags::NewFile;
	if (rlUnitConfig->overwrite)
		attachFlags |= AttachFlags::Overwrite;

    // Try to attach the specified file to this unit
    StatusCode result;
    if ((result = attachUnit (rlUnitConfig->fileName, attachFlags)) !=
        StatusCode::Success)
            return result;

    // Set the drive default write-protected if that is specified in
    // the configuration.
    if (rlUnitConfig->writeProtect)
    {
        setWriteProtected (true);
        driveStatus_ |= RLV12const::MPR_GS_WriteLock;
    }

    // Set the drive state as if the load procedure had already executed.

    // Position at cylinder 0
    currentDiskAddress_ = 0;

    spinUpTime_ = std::chrono::seconds {rlUnitConfig->spinUpTime};
    
    // Create a bad block table on a new disk image (if the 
    // image is not read-only)
    t_offset fileSize;
    if ((fileSize = attachedFileSize ()) == 0)
    {   
        // If read-only we're done
        if (isWriteProtected ())
            return StatusCode::Success;

        // Create a bad block table on the last track of the device.
        // The position of the last track is also based on the unit's
        // capacity!
        return createBadBlockTable (RLV12const::sectorsPerSurface, 
            RLV12const::wordsPerSector, capacity_);
    }

    // If auto-sizing is set, determine drive type on the file size
    if (rlStatus_ & RlStatus::UNIT_AUTO)
    {
        if (fileSize > (RLV12const::RL01_WordsPerCartridge * sizeof (u16)))
        {
            rlStatus_ |= RlStatus::UNIT_RL02;
            capacity_ = RLV12const::RL02_WordsPerCartridge;
            driveStatus_ |= RLV12const::MPR_GS_DriveType;
        }
        else
        {
            rlStatus_ &= ~Bitmask(RlStatus::UNIT_RL02);
            capacity_ = RLV12const::RL01_WordsPerCartridge;
        }
    }
    return StatusCode::Success;
}
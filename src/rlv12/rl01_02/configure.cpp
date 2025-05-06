#include "../rlv12.h"
#include "fio/fio.h"

#include <variant>

using std::shared_ptr;
using std::get;

StatusCode RL01_02::configure (shared_ptr<RLUnitConfig> rlUnitConfig)
{
    if (rlUnitConfig->fileName.empty ())
        return StatusCode::ArgumentError;

    Bitmask<AttachFlags> attachFlags =
        getAttachFlagsFromConfig (rlUnitConfig);

    // Try to attach the specified file to this unit
    StatusCode result;
    if ((result = attachUnit (rlUnitConfig->fileName, attachFlags)) !=
        StatusCode::Success)
        return result;

    // The file size will be the size of an existing file or zero if a
    // new file has been created
    t_offset fileSize = attachedFileSize ();

    // Set unit size and type from the configuration and actual file size
    setDriveParameters (rlUnitConfig->rlUnitType, fileSize);

    // Set the drive default write-protected if that is specified in
    // the configuration.
    if (rlUnitConfig->writeProtect)
    {
        setWriteProtected (true);
        driveStatus_ |= RLV12const::MPR_GS_WriteLock;
    }

    // Position at cylinder 0
    currentDiskAddress_ = 0;

    spinUpTime_ = std::chrono::seconds {rlUnitConfig->spinUpTime};
    
    // Create a bad block table on a new disk image (if the 
    // image is not read-only)
    if ((fileSize = attachedFileSize ()) == 0)
    {   
        // If read-only we're done
        if (isWriteProtected ())
            return StatusCode::Success;

        // Create a bad block table on the last track of the device.
        return createBadBlockTable (geometry_);
    }

    return StatusCode::Success;
}

Bitmask<AttachFlags> RL01_02::getAttachFlagsFromConfig (
    shared_ptr<RLUnitConfig> rlUnitConfig)
{
    Bitmask<AttachFlags> attachFlags {AttachFlags::Default};

    if (rlUnitConfig->writeProtect)
        attachFlags |= AttachFlags::ReadOnly;
    if (rlUnitConfig->newFile)
        attachFlags |= AttachFlags::NewFile;
    if (rlUnitConfig->overwrite)
        attachFlags |= AttachFlags::Overwrite;

    return attachFlags;
}

// Set unit type and size from the given drive type. Note that if
// the unit type is Auto the unit's capacity is determined after
// attaching the file to the unit. The capacity is also needed for
// creation of the bad block table, but the combination of the newFile
// option and Auto unit type is excluded in the configuration data.
//
void RL01_02::setDriveParameters (RLUnitConfig::RLUnitType unitType,
    t_offset fileSize)
{
    Geometry rl01Geometry {
        RLV12const::sectorsPerSurface,
        RLV12const::surfacesPerCylinder,
        RLV12const::RL01cylindersPerCartridge,
        RLV12const::wordsPerSector};

    Geometry rl02Geometry {
        RLV12const::sectorsPerSurface,
        RLV12const::surfacesPerCylinder,
        RLV12const::RL02cylindersPerCartridge,
        RLV12const::wordsPerSector};

    if (unitType == RLUnitConfig::RLUnitType::RL01)
    {
        geometry_ = rl01Geometry;
        capacity_ = RLV12const::RL01_WordsPerCartridge;
        rlStatus_ &= ~Bitmask (RlStatus::UNIT_RL02);
    }
    else if (unitType == RLUnitConfig::RLUnitType::RL02)
    {
        geometry_ = rl02Geometry;
        capacity_ = RLV12const::RL02_WordsPerCartridge;
        rlStatus_ |= Bitmask (RlStatus::UNIT_RL02);
        driveStatus_ |= RLV12const::MPR_GS_DriveType;
    }
    else if (unitType == RLUnitConfig::RLUnitType::Auto)
    {
        rlStatus_ |= Bitmask (RlStatus::UNIT_AUTO);

        // If auto-sizing is set, determine drive type on the file size
        if (fileSize > (RLV12const::RL01_WordsPerCartridge * sizeof (u16)))
        {
            geometry_ = rl02Geometry;
            rlStatus_ |= RlStatus::UNIT_RL02;
            capacity_ = RLV12const::RL02_WordsPerCartridge;
            driveStatus_ |= RLV12const::MPR_GS_DriveType;
        }
        else
        {
            geometry_ = rl01Geometry;
            rlStatus_ &= ~Bitmask (RlStatus::UNIT_RL02);
            capacity_ = RLV12const::RL01_WordsPerCartridge;
        }
    }
}

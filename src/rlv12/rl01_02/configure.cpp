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
    setDriveGeometry (rlUnitConfig->rlUnitType, fileSize);

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
    if (fileSize == 0)
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

// Set unit geometry from the configured drive type and possibly file size.
// If the configured drive type is "automatic" the drive type is determined
// from the file size.
//
void RL01_02::setDriveGeometry (RLUnitConfig::RLUnitType unitType,
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

    geometry_ = rl01Geometry;

    if (unitType == RLUnitConfig::RLUnitType::RL02 ||
       (unitType == RLUnitConfig::RLUnitType::Auto && 
            fileSize > (RLV12const::RL01_WordsPerCartridge * sizeof (u16))))
    {
        geometry_ = rl02Geometry;
        driveStatus_ |= RLV12const::MPR_GS_DriveType;
    }
}

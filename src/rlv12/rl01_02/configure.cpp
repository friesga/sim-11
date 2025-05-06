#include "../rlv12.h"
#include "fio/fio.h"

#include <variant>

using std::shared_ptr;
using std::get;

StatusCode RL01_02::configure (shared_ptr<RLUnitConfig> rlUnitConfig)
{
    if (rlUnitConfig->fileName.empty ())
        return StatusCode::ArgumentError;

    // Try to attach the specified file to this unit
    if (StatusCode result = 
            attachUnit (rlUnitConfig->fileName, getAttachMode (rlUnitConfig));
        result != StatusCode::Success)
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

    spinUpTime_ = std::chrono::seconds {rlUnitConfig->spinUpTime};
    
    // Create a bad block table on a new disk image (if the 
    // image is not read-only)
    if (fileSize == 0 && !isWriteProtected ())
        return createBadBlockTable (geometry_);

    return StatusCode::Success;
}

Bitmask<AttachFlags> RL01_02::getAttachMode (
    shared_ptr<RLUnitConfig> rlUnitConfig)
{
    Bitmask<AttachFlags> attachMode {AttachFlags::Default};

    if (rlUnitConfig->writeProtect)
        attachMode |= AttachFlags::ReadOnly;
    if (rlUnitConfig->newFile)
        attachMode |= AttachFlags::NewFile;
    if (rlUnitConfig->overwrite)
        attachMode |= AttachFlags::Overwrite;

    return attachMode;
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

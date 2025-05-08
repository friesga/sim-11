#include "../rlv12.h"
#include "fio/fio.h"

#include <variant>
#include <stdexcept>

using std::shared_ptr;
using std::get;
using std::invalid_argument;

StatusCode RL01_02::configure (shared_ptr<RLUnitConfig> rlUnitConfig)
{
    if (rlUnitConfig->fileName.empty ())
        return StatusCode::ArgumentError;

    // Now determine the actual drive type as soon as possible, to get
    // rid of the RLUnitType Auto and be able to determine the drive geometry.
    driveType_ = determineDriveType (rlUnitConfig);
    geometry_ = driveGeometry (driveType_);

    // Try to attach the specified file to this unit
    if (StatusCode result = attachFile (rlUnitConfig->fileName, geometry_,
            getAttachMode (rlUnitConfig));
        result != StatusCode::Success)
        return result;

    // Set the drive type in the MPR
    if (driveType_ == DriveType::RL02)
        driveStatus_ |= RLV12const::MPR_GS_DriveType;

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
    if (attachedFileSize () == 0 && !isWriteProtected ())
        return createBadBlockTable ();

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

// Get unit geometry from the given drive type
Geometry RL01_02::driveGeometry (DriveType driveType)
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

    return (driveType == DriveType::RL01) ? rl01Geometry : rl02Geometry;
}

RL01_02::DriveType RL01_02::determineDriveType (shared_ptr<RLUnitConfig> rlUnitConfig)
{
    switch (rlUnitConfig->rlUnitType)
    {
        case RLUnitConfig::RLUnitType::RL01:
            return DriveType::RL01;

        case RLUnitConfig::RLUnitType::RL02:
            return DriveType::RL02;

        case RLUnitConfig::RLUnitType::Auto:
            return (fileSize (rlUnitConfig->fileName) >
                    RLV12const::RL01_WordsPerCartridge * sizeof (u16)) ?
                DriveType::RL02 : DriveType::RL01;

        default:
           // Should not happen
            throw invalid_argument ("Unknown RLunitType");
    }
}

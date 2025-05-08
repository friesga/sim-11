#include "diskdrive.h"
#include "cmdlineoptions/cmdlineoptions.h"

StatusCode DiskDrive::attachUnit(std::string fileName, Geometry geometry,
    Bitmask<AttachFlags> attachMode)
{
    StatusCode statusCode;

    // Save the given geometry for this drive so it can be used to
    // calculate file positions.
    geometry_ = geometry;

    // Create a new file if specified
    if (attachMode & AttachFlags::NewFile)
        statusCode = createFile (fileName, attachMode);
    else
    {
        // Open existing file read/write 
        statusCode = openReadWrite (fileName);

        // If that doesn't succeed open the file read-only
        if (statusCode != StatusCode::Success)
            statusCode = openReadOnly (fileName);
    }

    if (statusCode != StatusCode::Success)
        return statusCode;

    return StatusCode::Success;
}

bool DiskDrive::isAttached () const
{
    return (diskFileStream != nullptr);
}

// Return the size of the attached file in bytes
t_offset DiskDrive::attachedFileSize () const
{
    return fio::fsize (diskFileStream);
}
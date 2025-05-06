#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"

StatusCode Unit::attachUnit(std::string fileName, Bitmask<AttachFlags> attachMode)
{
    StatusCode statusCode;

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

bool Unit::isAttached () const
{
    return (diskFileStream != nullptr);
}

// Return the size of the attached file in bytes
t_offset Unit::attachedFileSize () const
{
    return fio::fsize (diskFileStream);
}
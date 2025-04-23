#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"

StatusCode Unit::attachUnit(std::string fileName, Bitmask<AttachFlags> flags)
{
    StatusCode statusCode;

    // Create a new file if specified
    if (flags & AttachFlags::NewFile)
        statusCode = createFile (fileName, flags);
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

    unitStatus_ |= Status::UNIT_ATT;
    fileName_ = fileName;
    position_ = 0;
    return StatusCode::Success;
}
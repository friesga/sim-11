#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <iostream>
#include <sys/stat.h>

// Functions defined sim_fio library
extern FILE *sim_fopen (const char *file, const char *mode);
extern size_t sim_fread (void* bptr, size_t size, size_t count, FILE* fptr);

StatusCode Unit::attach_unit(std::string fileName, Bitmask<AttachFlags> flags)
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
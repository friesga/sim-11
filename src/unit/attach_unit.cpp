#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <iostream>
#include <sys/stat.h>

// Functions defined sim_fio library
extern FILE *sim_fopen (const char *file, const char *mode);
extern size_t sim_fread (void* bptr, size_t size, size_t count, FILE* fptr);

// Functions defined in lib
extern bool gotApproval(std::string question, bool defaultAnswer);

StatusCode Unit::attach_unit(std::string fileName, Bitmask<AttachFlags> flags)
{
    StatusCode statusCode;

    // Create a new file if specified
    if (flags & AttachFlags::NewFile)
        statusCode = createFile (fileName, flags);
    else 
        // Open existing file read/write 
        statusCode = openReadWrite (fileName);

    if (statusCode != StatusCode::OK)
        return statusCode;

    unitStatus_ |= Status::UNIT_ATT;
    fileName_ = fileName;
    position_ = 0;
    return StatusCode::OK;
}
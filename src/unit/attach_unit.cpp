#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <iostream>
#include <sys/stat.h>

// Functions defined sim_fio library
extern FILE *sim_fopen (const char *file, const char *mode);
extern size_t sim_fread (void* bptr, size_t size, size_t count, FILE* fptr);

// Functions defined in lib
extern bool gotApproval(std::string question, bool defaultAnswer);

StatusCode Unit::attach_unit(std::string fileName)
{
    struct stat info;
    StatusCode statusCode;

    // Check if a file can be attached to the unit
    if (!(flags_ & UNIT_ATTABLE))
        return StatusCode::NotAttachable;

    // ToDo: Check raw mode only?
    // if (dptr->flags & DEV_RAWONLY)
    //     return SCPE_NOFNC;

    // Create a new file if specified
    if (CmdLineOptions::get().createNewFile)
        statusCode = createFile (fileName);
    else 
    {    
        // Check if file exists and is a pipe 
        if (isPipe (fileName_))
            statusCode = openPipe (fileName);

        else if (CmdLineOptions::get().readOnly)
            statusCode = openReadOnly (fileName);
        
        else 
            // Open existing file read/write 
            statusCode = openReadWrite (fileName);
    }

    if (statusCode != StatusCode::OK)
        return statusCode;

    if (flags_ & UNIT_BUFABLE)
        setBuffered ();

    flags_ |= UNIT_ATT;
    fileName_ = fileName;
    position_ = 0;
    return StatusCode::OK;
}
#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <iostream>
#include <sys/stat.h>

// Functions defined sim_fio library
extern FILE* sim_fopen(const char* file, const char* mode);


// Functions defined in lib
extern bool gotApproval(std::string question, bool defaultAnswer);

StatusCode Unit::openReadWrite (std::string fileName)
{
    filePtr_ = sim_fopen(fileName.c_str(), "rb+");
    if (filePtr_ == NULL)
    {
        // Open for read/write failed
        if ((errno == EROFS) || (errno == EACCES))
        {
            // If the user specifies an existing file to open without
            // specifying the read-only file, he expects the file to be
            // opened for read/write. If that doesn't succeed offer to
            // open the file read-only, if that is allowed.
            if ((flags_ & (UNIT_RO | UNIT_ROABLE)) == 0)
                // Open for read/write failed and opening read-only
                // isn't possible
                return StatusCode::OpenError;

            if (!gotApproval("File cannot be opened read/write. Open read-only? [N]", false))
                return StatusCode::OpenError;

            // Try to open the file read-only
            filePtr_ = sim_fopen(fileName.c_str(), "rb");
            if (filePtr_ == NULL)
                return StatusCode::OpenError;

            flags_ |= UNIT_RO;
            if (!CmdLineOptions::get().quiet)
                std::cout << owningDevice_->name() << ": unit is read only\n";
        }

        return StatusCode::OpenError;
    }

    return StatusCode::OK;
}
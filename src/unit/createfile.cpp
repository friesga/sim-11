#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <iostream>
#include <sys/stat.h>

// Functions defined sim_fio library
extern FILE* sim_fopen(const char* file, const char* mode);

// Functions defined in lib
extern bool gotApproval(std::string question, bool defaultAnswer);

StatusCode Unit::createFile(std::string fileName, Bitmask<AttachFlags> flags)
{
    struct stat info;

    // Opening a file with write/update will cause existing files to be
    // overwritten, so check if a file with the specified name already
    // exists and if that's the case ask the user if it is ok to overwrite
    // the file.
    if (!stat (fileName.c_str(), &info) && !(flags & AttachFlags::Overwrite))
        return StatusCode::ArgumentError;

    filePtr_ = sim_fopen (fileName.c_str(), "wb+");
    if (filePtr_ == NULL)
        return StatusCode::OpenError;

    if (!CmdLineOptions::get().quiet)
        std::cout << owningDevice_->name() << ": creating new file\n";

    return StatusCode::OK;
}
#include "unit.h"
#include "../cmdlineoptions/cmdlineoptions.h"

#include <iostream>
#include <sys/stat.h>

// Functions defined sim_fio library
extern FILE* sim_fopen(const char* file, const char* mode);

StatusCode Unit::openReadOnly()
{
    // Open regular file. Read only allowed?
    if ((flags_ & (UNIT_RO | UNIT_ROABLE)) == 0)
        return StatusCode::ReadOnlyNotAllowed;

    // Open file read-only
    filePtr_ = sim_fopen (fileName_.c_str(), "rb");
    if (filePtr_ == NULL)
        return StatusCode::OpenError;

    // Set unit read-only
    flags_ |= UNIT_RO;

    if (!CmdLineOptions::get().quiet)
        std::cout << owningDevice_->name() << ": unit is read only\n";

    return StatusCode::OK;
}
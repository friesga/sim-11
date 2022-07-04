#include "unit.h"

#include <sys/stat.h>

// Functions defined sim_fio library
extern FILE* sim_fopen(const char* file, const char* mode);

StatusCode Unit::openPipe()
{
    // Check the init is sequential
    if (flags_ & UNIT_SEQ)
    {
        // If the unit is readable,
        if (flags_ & (UNIT_RO | UNIT_ROABLE))
            // then open the pipe for reading,
            filePtr_ = sim_fopen (fileName_.c_str(), "rb");
        else
            // open the pipe for writing
            filePtr_ = sim_fopen (fileName_.c_str(), "wb");

        // If the file failed to open then report the error
        if (filePtr_ == NULL)
            return StatusCode::OpenError;

        // Otherwise set the pipe flag
        dynflags_ |= UNIT_PIPE;
        return StatusCode::OK;
    }
    else
        // Otherwise the unit is not sequential 
        // so it cannot be attached to a pipe
        return StatusCode::FunctionNotImplemented;
}
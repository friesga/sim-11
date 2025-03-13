#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"
#include "fio/fio.h"

#include <sys/stat.h>   // Required for stat

StatusCode Unit::createFile(std::string fileName, Bitmask<AttachFlags> flags)
{
    struct stat info;

    // Opening a file with write/update will cause existing files to be
    // overwritten, so check if a file with the specified name already
    // exists and if that's the case ask the user if it is ok to overwrite
    // the file.
    if (!stat (fileName.c_str(), &info) && !(flags & AttachFlags::Overwrite))
        return StatusCode::ArgumentError;

    filePtr_ = fio::fopen (fileName.c_str(), "wb+");
    if (filePtr_ == NULL)
        return StatusCode::OpenError;

     Logger::instance() << owningDevice_->name() + ": creating new file\n";

    return StatusCode::Success;
}
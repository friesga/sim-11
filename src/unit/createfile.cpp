#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"
#include "fio/fio.h"

#include <sys/stat.h>   // Required for stat

StatusCode Unit::createFile(std::string fileName, Bitmask<AttachFlags> attachMode)
{
    struct stat info;

    // Opening a file with write/update will cause existing files to be
    // overwritten, so check if a file with the specified name already
    // exists and if that's the case ask the user if it is ok to overwrite
    // the file.
    if (!stat (fileName.c_str(), &info) && !(attachMode & AttachFlags::Overwrite))
        return StatusCode::ArgumentError;

    diskFileStream = fio::fopen (fileName.c_str(), "wb+");
    if (diskFileStream == NULL)
        return StatusCode::OpenError;

    return StatusCode::Success;
}
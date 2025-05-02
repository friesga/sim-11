#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"

#include "fio/fio.h"

StatusCode Unit::openReadOnly(std::string fileName)
{
    // Open file read-only
    diskFileStream = fio::fopen (fileName.c_str(), "rb");

    if (diskFileStream == NULL)
        return StatusCode::OpenError;

    // Set unit read-only
    writeProtected_ = true;

    return StatusCode::Success;
}
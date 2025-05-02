#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"

#include "fio/fio.h"

StatusCode Unit::openReadOnly(std::string fileName)
{
    // Open file read-only
    filePtr_ = fio::fopen (fileName.c_str(), "rb");

    if (filePtr_ == NULL)
        return StatusCode::OpenError;

    // Set unit read-only
    writeProtected_ = true;

    return StatusCode::Success;
}
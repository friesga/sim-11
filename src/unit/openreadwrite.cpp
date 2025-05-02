#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"
#include "fio/fio.h"

StatusCode Unit::openReadWrite (std::string fileName)
{
    diskFileStream = fio::fopen(fileName.c_str(), "rb+");
    if (diskFileStream == NULL)
        return StatusCode::OpenError;

    return StatusCode::Success;
}
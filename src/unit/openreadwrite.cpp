#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"
#include "fio/fio.h"

StatusCode Unit::openReadWrite (std::string fileName)
{
    filePtr_ = fio::fopen(fileName.c_str(), "rb+");
    if (filePtr_ == NULL)
        return StatusCode::OpenError;

    return StatusCode::Success;
}
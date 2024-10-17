#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"

#include <iostream>
#include <sys/stat.h>

// Functions defined sim_fio library
extern FILE* sim_fopen(const char* file, const char* mode);

StatusCode Unit::openReadOnly(std::string fileName)
{
    // Open file read-only
    filePtr_ = sim_fopen (fileName.c_str(), "rb");
    if (filePtr_ == NULL)
        return StatusCode::OpenError;

    // Set unit read-only
    unitStatus_ |= Status::WRITE_PROTECT;

    Logger::instance() << owningDevice_->name() + ": unit is read only\n";

    return StatusCode::OK;
}
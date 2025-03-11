#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"

#include <iostream>
#include <sys/stat.h>

// Functions defined sim_fio library
extern FILE* sim_fopen(const char* file, const char* mode);

StatusCode Unit::openReadWrite (std::string fileName)
{
    filePtr_ = sim_fopen(fileName.c_str(), "rb+");
    if (filePtr_ == NULL)
        return StatusCode::OpenError;

    return StatusCode::Success;
}
#include "cartridge.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"

#include "fio/fio.h"

StatusCode Cartridge::openReadOnly(std::string fileName)
{
    // Open file read-only
    diskFileStream_ = fio::fopen (fileName.c_str(), "rb");

    if (diskFileStream_ == NULL)
        return StatusCode::OpenError;

    // Set unit read-only
    writeProtected_ = true;

    return StatusCode::Success;
}
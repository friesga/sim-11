#include "cartridge.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"
#include "fio/fio.h"

#include <sys/stat.h>   // Required for stat

StatusCode Cartridge::createFile (std::string fileName)
{
    diskFileStream = fio::fopen (fileName.c_str(), "wb+");
    if (diskFileStream == NULL)
        return StatusCode::OpenError;

    // Extend the file to the drive's capacity
    fseek (diskFileStream, geometry_.wordCapacity () * sizeof (u16), SEEK_SET);
    fputc ('\0', diskFileStream);

    return StatusCode::Success;
}
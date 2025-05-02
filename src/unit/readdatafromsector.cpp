#include "unit.h"
#include "logger/logger.h"

size_t Unit::readDataFromSector (int32_t offset, u16* buffer, u32 wordCount)
{
    // Set position in file to the block to be read
    if (fseek (diskFileStream, offset, SEEK_SET) != 0)
    {
        Logger::instance () << "fseek error in Unit::readDataFromSector";
        return 0;
    }

    // Read wordCount * 2 bytes; returned is the number of bytes read 
    size_t numBytes = fread (buffer, sizeof (int16_t), wordCount, diskFileStream);

    if (ferror (diskFileStream))
    {
        Logger::instance () << "fread error in Unit::readDataFromSector";
        return 0;
    }

    return numBytes;
}
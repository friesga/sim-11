#include "unit.h"
#include "logger/logger.h"

size_t Unit::writeDataToSector (int32_t offset, u16* buffer, size_t numWords)
{
    if (writeProtected_)
        return 0;

    // Set position in file to the block to be written
    if (fseek (diskFileStream, offset, SEEK_SET) != 0)
    {
        Logger::instance () << "fseek error in Unit::writeDataToSector";
        return 0;
    }

    size_t numBytes = fwrite (buffer, sizeof (int16_t), numWords, diskFileStream);

    if (ferror (diskFileStream))
        Logger::instance () << "fwrite error in Unit::writeDataToSector";

    return numBytes;
}
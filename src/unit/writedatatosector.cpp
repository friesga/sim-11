#include "unit.h"
#include "logger/logger.h"

size_t Unit::writeDataToSector (int32_t offset, u16* buffer, size_t numWords)
{
    // Set position in file to the block to be written
    if (fseek (filePtr_, offset, SEEK_SET) != 0)
    {
        Logger::instance () << "fseek error in Unit::writeDataToSector";
        return 0;
    }

    size_t numBytes = fwrite (buffer, sizeof (int16_t), numWords, filePtr_);

    if (ferror (filePtr_))
        Logger::instance () << "fwrite error in Unit::writeDataToSector";

    return numBytes;
}
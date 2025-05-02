#include "unit.h"
#include "logger/logger.h"

size_t Unit::readDataFromSector (int32_t offset, u16* buffer, u32 wordCount)
{
    // Set position in file to the block to be read
    if (fseek (filePtr_, offset, SEEK_SET) != 0)
    {
        Logger::instance () << "Seek error in RL01_02::readData";
        return 0;
    }

    // Read wordCount * 2 bytes; returned is the number of bytes read 
    size_t numBytes = fread (buffer, sizeof (int16_t), wordCount, filePtr_);

    if (ferror (filePtr_))
    {
        Logger::instance () << "Read error in RL01_02::readData";
        return 0;
    }

    return numBytes;
}
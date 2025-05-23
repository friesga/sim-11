#include "diskdrive.h"
#include "logger/logger.h"

size_t DiskDrive::readDataFromSector (DiskAddress diskAddress, u16* buffer,
    u32 wordCount)
{
    // Set position in file to the block to be read
    if (fseek (diskFileStream, filePosition (diskAddress), SEEK_SET) != 0)
    {
        Logger::instance () << "fseek error in DiskDrive::readDataFromSector";
        return 0;
    }

    // Read wordCount * 2 bytes; returned is the number of bytes read 
    size_t wordsRead = fread (buffer, sizeof (int16_t), wordCount, diskFileStream);

    if (ferror (diskFileStream))
    {
        Logger::instance () << "fread error in DiskDrive::readDataFromSector";
        return 0;
    }

    return wordsRead;
}
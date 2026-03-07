#include "diskdrive.h"
#include "logger/logger.h"

#include <algorithm>

using std::min;

size_t DiskDrive::writeDataToSector (DiskAddress diskAddress, u16* buffer,
    u32 numWords)
{
    if (writeProtected_)
        return 0;

    // Set position in file to the block to be written
    if (fseek (diskFileStream, byteOffset (diskAddress), SEEK_SET) != 0)
    {
        Logger::instance () << "fseek error in DiskDrive::writeDataToSector";
        return 0;
    }

    // Write wordCount * 2 bytes, avoiding a write beyond the disk's capacity;
    // returned is the number of objects (i.e. words) written.
    size_t wordsWritten = fwrite (buffer, sizeof (int16_t),
        min (geometry_.wordCapacity () - wordOffset (diskAddress), numWords),
        diskFileStream);

    if (ferror (diskFileStream))
        Logger::instance () << "fwrite error in DiskDrive::writeDataToSector";

    return wordsWritten;
}
#include "diskdrive.h"

u32 DiskDrive::wordOffset (DiskAddress diskAddress) const
{
    return geometry_.LBN (diskAddress) * geometry_.wordsPerSector ();
}

u32 DiskDrive::byteOffset (DiskAddress diskAddress) const
{
    return wordOffset (diskAddress) * sizeof (int16_t);
}

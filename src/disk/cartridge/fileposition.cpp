#include "cartridge.h"

u32 Cartridge::wordOffset (DiskAddress diskAddress) const
{
    return geometry_.LBN (diskAddress) * geometry_.wordsPerSector ();
}

u32 Cartridge::byteOffset (DiskAddress diskAddress) const
{
    return wordOffset (diskAddress) * sizeof (int16_t);
}

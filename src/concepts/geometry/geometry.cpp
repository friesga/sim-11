#include "geometry.h"

Geometry::Geometry (u16 sectorsPerSurface, u16 numberOfHeads,
    u16 cylindersPerDisk, u16 wordsPerSector)
    :
    sectorsPerSurface_ {sectorsPerSurface},
    numberOfHeads_ {numberOfHeads},
    cylindersPerDisk_ {cylindersPerDisk},
    wordsPerSector_ {wordsPerSector},
    diskCapacity_ {static_cast<u32> (cylindersPerDisk * numberOfHeads *
        sectorsPerSurface)},
    wordCapacity_ {static_cast<u32> (diskCapacity_ * wordsPerSector)}
{}

u16 Geometry::sectorsPerSurface () const
{
    return sectorsPerSurface_;
}

u16 Geometry::numberOfHeads () const
{
    return numberOfHeads_;
}

u16 Geometry::cylindersPerDisk () const
{
    return cylindersPerDisk_;
}

u16 Geometry::wordsPerSector () const
{
    return wordsPerSector_;
}

u32 Geometry::diskCapacity () const
{
    return diskCapacity_;
}

u32 Geometry::wordCapacity () const
{
    return wordCapacity_;
}
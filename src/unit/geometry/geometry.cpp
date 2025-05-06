#include "geometry.h"

Geometry::Geometry (u16 sectorsPerSurface, u16 numberOfHeads, u16 cylindersPerDisk,
    u16 wordsPerSector)
    :
    sectorsPerSurface_ {sectorsPerSurface},
    numberOfHeads_ {numberOfHeads},
    cylindersPerDisk_ {cylindersPerDisk},
    wordsPerSector_ {wordsPerSector},
    wordCapacity_ {cylindersPerDisk * numberOfHeads *
        sectorsPerSurface * wordsPerSector}
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

s32 Geometry::wordCapacity () const
{
    return wordCapacity_;
}
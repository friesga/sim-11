#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "types.h"

class Geometry
{
public:
    Geometry (u16 sectorsPerSurface, u16 numberOfHeads, u16 cylindersPerDisk,
        u16 wordsPerSector);
    u16 sectorsPerSurface () const;
    u16 numberOfHeads () const;
    u16 cylindersPerDisk () const;
    u16 wordsPerSector () const;
    s32 capacityInBytesytes () const;

private:
    u16 sectorsPerSurface_ {};
    u16 numberOfHeads_ {};
    u16 cylindersPerDisk_ {};
    u16 wordsPerSector_ {};
    s32 capacityInBytesytes_ {};
};


#endif // _GEOMETRY_H_

#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "types.h"
#include "diskdrive/diskaddress/diskaddress.h"

class Geometry
{
public:
    Geometry () = default;
    Geometry (u16 sectorsPerSurface, u16 numberOfHeads, u16 cylindersPerDisk,
        u16 wordsPerSector);
    u16 sectorsPerSurface () const;
    u16 numberOfHeads () const;
    u16 cylindersPerDisk () const;
    u16 wordsPerSector () const;
    u32 wordCapacity () const;

    u32 LBN (DiskAddress diskAddress) const;
    DiskAddress lbnTodiskAddress (u32 lbn) const;

private:
    u16 sectorsPerSurface_ {};
    u16 numberOfHeads_ {};
    u16 cylindersPerDisk_ {};
    u16 wordsPerSector_ {};
    u32 wordCapacity_ {};
};


#endif // _GEOMETRY_H_

#ifndef _DISKADDRESS_H
#define _DISKADDRESS_H

struct DiskAddress
{
    u16 sector {0};
    u16 head {0};
    u16 cylinder {0};

    // Definition of the equality operator to facilitate the unit tests
    //
    bool operator== (const DiskAddress& other) const
    {
        return sector == other.sector && head == other.head &&
            cylinder == other.cylinder;
    }
};

#endif // _DISKADDRESS_H
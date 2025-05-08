#ifndef _DISKADDRESS_H
#define _DISKADDRESS_H

struct DiskAddress
{
    u16 sector {0};
    u16 head {0};
    u16 cylinder {0};
};

#endif _DISKADDRESS_H
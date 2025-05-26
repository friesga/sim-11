#ifndef _DISKDRIVE_H_
#define _DISKDRIVE_H_

#include "abstractbusdevice/abstractbusdevice.h"
#include "statuscodes.h"
#include "types.h"
#include "bitmask.h"
#include "attachflags.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "fio/fio.h"
#include "geometry/geometry.h"
#include "diskaddress/diskaddress.h"

#include <cstdio>
#include <string>
#include <memory>

using std::shared_ptr;

// Definition of the functions common to all disk drives. The class offers
// common functions for the support of disk drives:
// 
// - Attaching a file to the disk drive,
// - Reading/write a sector to the disk, thereby translating the disk address
//   to a position in the attached file,
// - Keeping track of write protect status
// - Creation of a bad block table on the drive.
//
// Disk images are represented in the simh file format, i.e. as unstructured
// binary files consisting of 16 bit data items. Sectors are numbered from
// sector 0 of surface 0 of cylinder 0 to the last sector on the disk. This
// allows easy transcription of real disks to files useable by a simulator.
//
class DiskDrive
{
public:
    StatusCode attachFile (std::string fileName, Geometry geometry,
        Bitmask<AttachFlags> attachMode);
    bool isAttached () const;
    t_offset attachedFileSize () const;
    size_t readDataFromSector (DiskAddress diskAddress, u16* buffer,
        u32 wordCount);
    size_t writeDataToSector (DiskAddress diskAddress, u16* buffer,
        size_t numWords);
    void setWriteProtected (bool writeProtected);
    bool isWriteProtected () const;
    StatusCode createBadBlockTable ();

private:
    Geometry geometry_ {};
    bool writeProtected_ {false};
    FILE* diskFileStream {nullptr};

    StatusCode createFile (std::string fileName, Bitmask<AttachFlags> attachMode);
    StatusCode openReadOnly (std::string fileName);
    StatusCode openReadWrite (std::string fileName);
    u32 LBN (DiskAddress diskAddress) const;
    u32 wordOffset (DiskAddress diskAddress) const;
    u32 byteOffset (DiskAddress diskAddress) const;
};

#endif // !_DISKDRIVE_H_

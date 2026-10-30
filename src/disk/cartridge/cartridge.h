#ifndef _CARTRIDGE_H_
#define _CARTRIDGE_H_

#include "abstractbusdevice/abstractbusdevice.h"
#include "statuscodes.h"
#include "types.h"
#include "bitmask.h"
#include "attachflags.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "fio/fio.h"
#include "concepts/geometry/geometry.h"
#include "concepts/diskaddress/diskaddress.h"

#include <cstdio>
#include <string>

// Definition of the functions for a cartridge:
// 
// - Attaching a file to the cartridge,
// - Reading/write a sector to the cartridge, thereby translating the disk address
//   to a position in the attached file,
// - Keeping track of write protect status
// - Creation of a bad block table on the catridge.
//
// Disk images are represented in the simh file format, i.e. as unstructured
// binary files consisting of 16 bit data items. Sectors are numbered from
// sector 0 of surface 0 of cylinder 0 to the last sector on the disk. This
// allows easy transcription of real disks to files useable by a simulator.
//
class Cartridge
{
public:
    StatusCode attachFile (std::string fileName, Geometry geometry,
        Bitmask<AttachFlags> attachMode);
    bool isAttached () const;
    t_offset attachedFileSize () const;
    size_t readDataFromSector (DiskAddress diskAddress, u16* buffer,
        u32 wordCount);
    size_t writeDataToSector (DiskAddress diskAddress, u16* buffer,
        u32 numWords);
    void setWriteProtected (bool writeProtected);
    bool isWriteProtected () const;
    StatusCode createBadBlockTable ();
    u32 LBN (DiskAddress diskAddress) const;

private:
    Geometry geometry_ {};
    bool writeProtected_ {false};
    FILE* diskFileStream {nullptr};

    StatusCode createFile (std::string fileName, Bitmask<AttachFlags> attachMode);
    StatusCode openReadOnly (std::string fileName);
    StatusCode openReadWrite (std::string fileName);
    
    u32 wordOffset (DiskAddress diskAddress) const;
    u32 byteOffset (DiskAddress diskAddress) const;
};

#endif // !_CARTRIDGE_H_

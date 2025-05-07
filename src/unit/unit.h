#ifndef _UNIT_H_
#define _UNIT_H_

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

// Definition of an abstract base class for the units of a device
//
// Disk images are represented in the simh file format, i.e. as unstructured
// binary files consisting of 16 bit data items. Sectors are numbered from
// sector 0 of surface 0 of cylinder 0 to the last sector on the disk. This
// allows easy transcription of real disks to files usable by a simulator.
//
class Unit
{
public:
    StatusCode attachUnit (std::string fileName, Bitmask<AttachFlags> attachMode);
    bool isAttached () const;
    t_offset attachedFileSize () const;
    size_t readDataFromSector (int32_t offset, u16* buffer, u32 wordCount);
    size_t writeDataToSector (int32_t offset, u16* buffer, size_t numWords);
    void setWriteProtected (bool writeProtected);
    bool isWriteProtected () const;
    StatusCode createBadBlockTable (Geometry geometry);
    int32_t filePosition (Geometry geometry, DiskAddress diskAddress) const;

private:
    bool writeProtected_ {false};
    FILE* diskFileStream {nullptr};

    StatusCode createFile (std::string fileName, Bitmask<AttachFlags> attachMode);
    StatusCode openReadOnly (std::string fileName);
    StatusCode openReadWrite (std::string fileName);
};

#endif // !_UNIT_H_

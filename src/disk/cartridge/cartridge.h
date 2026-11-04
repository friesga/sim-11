#ifndef _CARTRIDGE_H_
#define _CARTRIDGE_H_

#include "abstractbusdevice/abstractbusdevice.h"
#include "statuscodes.h"
#include "types.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "fio/fio.h"
#include "concepts/geometry/geometry.h"
#include "concepts/diskaddress/diskaddress.h"

#include <cstdio>
#include <string>
#include <memory>

// Definition of the functions for a cartridge:
// 
// - Attaching a file to the cartridge,
// - Reading/write a sector to the cartridge, thereby translating the disk address
//   to a position in the attached file,
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
    // The Cartridge supports just two modes to open a file: opening an
    // existing file read/write and the creation of a new file. The write
    // protection of the cartridge has to be provided by the unit as a user
    // can set the write protect status of a unit while no cartridge is
    // loaded.
    //
    enum class OpenMode
    {
        ReadWrite,                  // Open existing file read/write
        NewFile                     // Create new file
    };

    // Factory function to create a new Cartridge object. The function returns a
    // unique_ptr to the newly created object. The function returns a nullptr if
    // the file could not be opened or created.
    static std::unique_ptr<Cartridge> create (std::string fileName,
        Geometry geometry, OpenMode openMode);

    ~Cartridge ();
    t_offset attachedFileSize () const;
    size_t readDataFromSector (DiskAddress diskAddress, u16* buffer,
        u32 wordCount);
    size_t writeDataToSector (DiskAddress diskAddress, u16* buffer,
        u32 numWords);
    StatusCode createBadBlockTable ();
    u32 LBN (DiskAddress diskAddress) const;

private:
    // The constructor is defined private to force the use of the create()
    // function to create a new Cartridge object.
    Cartridge (std::string fileName, Geometry geometry,
        OpenMode openMode);

    Geometry geometry_ {};
    FILE* diskFileStream_ {nullptr};

    StatusCode createFile (std::string fileName);
    StatusCode openReadOnly (std::string fileName);
    StatusCode openReadWrite (std::string fileName);
    
    u32 wordOffset (DiskAddress diskAddress) const;
    u32 byteOffset (DiskAddress diskAddress) const;
};

#endif // !_CARTRIDGE_H_

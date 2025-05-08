#include "diskdrive.h"

// Calculate the position of a sector as an offset in the file from
// the specified diskAddress.
// 
// To avoid confusion let's get the definitions for the disk geometry
// straight (based on
// https://en.wikipedia.org/wiki/Cylinder-head-sector):
// 
// - Head selects a circular surface, i.e. a specific side of one of the
//   platters in the disk,
// - Cylinder is a cylindrical intersection through the stack of platters
//   in a disk,
// - The combination of cylinder and head is called track,
// - A track is subdivided into several equally-sized portions called sectors.
// 
// Cylinder, head and sector together form a coordinate system for data on
// a disk.
// 
// The given disk address is converted to a logical block number by means
// of the following formula:
// LBN = (cyl * numHeads + head) * numSectorsPerSurface + sector
//
// The LBN is then converted to a file position by the formula:
// darToDiskAddress = LBN * wordsPerSector * numBitsPerWord
//
// As the PDP-11 is a 16-bit machine the value for numBitsPerWord is 16.
// 
// An alternative formula for the calculation of the LBN resulting in the
// same mapping is:
// LBN = track * numSectorsPerSurface + sector.
// 
// This definition can be used if a disk address register contains bits
// for cylinder and head, situated side by side with the lower bits defining
// the head.
//
int32_t DiskDrive::filePosition (DiskAddress diskAddress) const
{
    u16 LBN = (diskAddress.cylinder * geometry_.numberOfHeads () + 
        diskAddress.head) * geometry_.sectorsPerSurface () + diskAddress.sector;

    return LBN * geometry_.wordsPerSector () * sizeof (int16_t);
}
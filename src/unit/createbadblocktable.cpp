// Factory bad block table creation routine
//
// This routine writes a DEC standard 044 compliant bad block table on the
// last track of the specified unit. The track contains a factory written
// bad block table, followed by a field written bad block table.
// 
// The first sector of both the factory bad block table and the field bad
// block table has the following format:
// word 0:      5 most significant octal digits of pack serial number
// word 1:      5 least significant octal digits of pack serial number
// words 2-3:   zeroes
// words 4-5:   address of bad sector
// words n-n+1: end of table (all ones)
//
// A sector in the bad block table comprises 256 words; this size may
// vary from the physical sector size used for particular unit types.
// A RL01/RL02 e.g. uses a sector size of 128 words.
// 
// The information in the first sector of the bad block table is duplicated
// in sectors 2, 4, 6 and 8 of that table. Sectors 1, 3, 5, 7 and 9 comprise
// all ones.
// 
// This implies that every sector with bad block information in a bad block
// table in the format specified above, is followed by sector containing all
// ones. The function creates an empty factory and an empty factory bad block
// table by writing 10 blocks of two sectors of 512 words; the first
// sector in the format specified above and the second sector comprising all
// ones.

#include "unit.h"
#include "statuscodes.h"
#include "fio/fio.h"

#include <stdexcept>

using std::logic_error;

StatusCode Unit::createBadBlockTable (int32_t sectorsPerSurface, 
    int32_t physWordsPerSector, u32 capacity)
{
    u32 da;
    u16 *badSectorInfo;
    // Each block written to the bad block file comprises two 256-word
    // sectors
    static int32_t const badSectorFileBlockSize = 256 * 2;
    static int32_t const physSectorsPerInfoBlock = 
        badSectorFileBlockSize / physWordsPerSector;

    if ((sectorsPerSurface < 2) || (physWordsPerSector < 16))
        return StatusCode::ArgumentError;

    if (!isAttached ())
        return StatusCode::UnAttached;

    if (writeProtected_)
        return StatusCode::ReadOnly;

    // Don't ask for approval to overwrite the last track as this function
    // will only be executed for newly created files.

    // Position file at last track
    da = (capacity - (sectorsPerSurface * physWordsPerSector)) * sizeof(u16);
    if (fio::fseek (filePtr_, da, SEEK_SET))
        return StatusCode::IOError;

    // Allocate a buffer of physWordsPerSector u16's
    // Use new() instead of make_unique as make_unique has no nothrow version.
    if ((badSectorInfo = new (std::nothrow) 
            u16[badSectorFileBlockSize]()) == nullptr)
        throw logic_error ("Allocating memory for bad block table failed");

    // Fill the block conform the standard specified above
    badSectorInfo[0] = 067000u;
    badSectorInfo[1] = 012345u;
    badSectorInfo[2] = badSectorInfo[3] = 0;
    for (size_t wordNr = 4; wordNr < badSectorFileBlockSize; ++wordNr)
        badSectorInfo[wordNr] = 0177777u;

    // Write the bad block tables
    for (int32_t blockNr = 0; 
        blockNr < 10 && blockNr * physSectorsPerInfoBlock < sectorsPerSurface;
        ++blockNr)
            fio::fwrite(badSectorInfo, sizeof(u16), 
                badSectorFileBlockSize, filePtr_);

    // Clean up.
    delete[] badSectorInfo;
    if (ferror (filePtr_))
        return StatusCode::IOError;

    return StatusCode::Success;
}

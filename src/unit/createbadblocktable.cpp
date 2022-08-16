/* 
   Factory bad block table creation routine

   This routine writes a DEC standard 044 compliant bad block table on the
   last track of the specified unit.  The bad block table consists of 10
   repetitions of the same table, formatted as follows:

        words 0-1       pack id number
        words 2-3       cylinder/sector/surface specifications
         :
        words n-n+1     end of table (-1,-1)

   Inputs:
        uptr    =       pointer to unit
        sectorsPerSurface     =       number of sectors per surface
        wordsPerSector     =       number of words per sector
   Outputs:
        sta     =       status code
*/

#include "unit.h"
#include "statuscodes.h"
#include "sim_fio/sim_fio.h"

extern bool gotApproval(std::string question, bool defaultAnswer);

StatusCode Unit::createBadBlockTable (int32_t sectorsPerSurface, 
    int32_t wordsPerSector)
{
    int32_t i;
    t_addr da;
    u16 *buf;

    if ((sectorsPerSurface < 2) || (wordsPerSector < 16))
        return StatusCode::ArgumentError;

    if (!(unitStatus_ & Status::UNIT_ATT))
        return StatusCode::UnAttached;

    if (unitStatus_ & Status::UNIT_RO)
        return StatusCode::ReadOnly;

    // Don't ask for approval to overwrite the last track as this function
    // will only be executed for newly created files.
    // if (!gotApproval ("Overwrite last track? [N]", false))
    //     return StatusCode::OK;

    da = (capacity_ - (sectorsPerSurface * wordsPerSector)) * sizeof(u16);

    if (sim_fseek (filePtr_, da, SEEK_SET))
        return StatusCode::IOError;

    // Allocate a buffer of wordsPerSector u16's
    // if ((buf = (uint16*) malloc(wordsPerSector * sizeof(uint16))) == NULL)
    if ((buf = new (std::nothrow) u16[wordsPerSector]()) == nullptr)
        throw ("Allocating memory for bad block table failed");

    buf[0] = buf[1] = 012345u;
    buf[2] = buf[3] = 0;
    for (i = 4; i < wordsPerSector; i++)
        buf[i] = 0177777u;
    for (i = 0; (i < sectorsPerSurface) && (i < 10); i++)
        sim_fwrite(buf, sizeof(u16), wordsPerSector, filePtr_);

    delete[] buf;
    if (ferror (filePtr_))
        return StatusCode::IOError;

    return StatusCode::OK;
}

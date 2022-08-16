#include "unit.h"
#include "cmdlineoptions/cmdlineoptions.h"

#include <iostream>
#include <sys/stat.h>

// Functions defined sim_fio library
extern size_t sim_fread(void* bptr, size_t size, size_t count, FILE* fptr);

void Unit::setBuffered()
{
    // aincr is the increment between device addresses, normally 1;
     // however, byte addressed devices with 16-bit words specify 2,
     // with 32-bit words 4.
     // See http://simh.trailing-edge.com/docs/simh.pdf
     // As for the PDP-11 the address increment is 1, the effective size
     // equals the defined capacity.
     // uint32 cap = ((uint32)uptr->capac) / dptr->aincr;  /* effective size */

     // The I/O must be buffered?
    if (unitStatus_ & Status::UNIT_MUSTBUF)
        // Allocate memory for buffered I/O. SZ_D hides a complex
        // calculation of the word (?) size. For the RLV12 this 
        // comes down to 16, so we allocate the buffer in u16's.
        // uptr->filebuf = calloc(cap, SZ_D(dptr));
        fileBuffer_ = new (std::nothrow) u16[capacity_]();

    if (fileBuffer_ == NULL)
        throw ("Allocating memory for buffered I/O failed");

    if (!CmdLineOptions::get().quiet)
        std::cout << owningDevice_->name() << ": buffering file in memory\n";

    hwmark_ = sim_fread(fileBuffer_, sizeof(u16), capacity_, filePtr_);

    // Mark unit buffered
    unitStatus_ |= Status::UNIT_BUF;
}
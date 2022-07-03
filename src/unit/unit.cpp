#include "unit.h"
#include "../cmdlineoptions/cmdlineoptions.h"

#include <iostream>
#include <sys/stat.h>

// Functions defined sim_fio library
extern FILE *sim_fopen (const char *file, const char *mode);
extern size_t sim_fread (void* bptr, size_t size, size_t count, FILE* fptr);

// Unit constructor
// Set default values for flags
Unit::Unit (Device *owningDevice)
    :
    owningDevice_ {owningDevice},
    flags_ {UNIT_ATTABLE}
{}

StatusCode Unit::attach_unit(std::string fileName)
{
    // DEVICE* dptr;
    struct stat info;

    // Check if a file can be attached to the unit
    if (!(flags_ & UNIT_ATTABLE))
        return StatusCode::NotAttachable;

    // if ((dptr = find_dev_from_unit(uptr)) == NULL)
    //     return SCPE_NOATT;

    // ToDo: Check raw mode only?
    // if (dptr->flags & DEV_RAWONLY)
    //     return SCPE_NOFNC;

    // Save filename
    fileName_ = fileName;

    // Check if file exists and is a pipe 
    if ((!stat (fileName_.c_str(), &info)) && (info.st_mode & _S_IFIFO))
    {
        // Check the init is sequential
        if (flags_ & UNIT_SEQ)
        {
            // If the unit is readable,
            if (flags_ & (UNIT_RO | UNIT_ROABLE))
                // then open the pipe for reading,
                filePtr_ = sim_fopen (fileName_.c_str(), "rb");
            else
                // open the pipe for writing
                filePtr_ = sim_fopen (fileName_.c_str(), "wb");

            // If the file failed to open then report the error
            if (filePtr_ == NULL)                          
                return StatusCode::OpenError;
            else 
                // Otherwise set the pipe flag
                dynflags_ |= UNIT_PIPE; 
        }
        else   
            // Otherwise the unit is not sequential 
            // so it cannot be attached to a pipe
            return StatusCode::FunctionNotImplemented;
    }
    else if (CmdLineOptions::get().readOnly)
    {                 
        // Read only allowed?
        if ((flags_ & (UNIT_RO | UNIT_ROABLE)) == 0)
            return StatusCode::ReadOnlyNotAllowed;

        // Open file read-only
        filePtr_ = sim_fopen (fileName_.c_str(), "rb");
        if (filePtr_ == NULL)                     
            return StatusCode::OpenError;

        // Set unit read-only
        flags_ |= UNIT_RO;
        if (!CmdLineOptions::get().quiet)
            std::cout << owningDevice_->name() << ": unit is read only\n";
    }
    else if (CmdLineOptions::get().createNewFile)
    {                 
        filePtr_ = sim_fopen (fileName_.c_str(), "wb+");
        if (filePtr_ == NULL)
            return StatusCode::OpenError;
        if (!CmdLineOptions::get().quiet)
            std::cout << owningDevice_->name() << ": creating new file\n";
    }
    else
    {
        // Open file read/write 
        filePtr_ = sim_fopen (fileName_.c_str(), "rb+");
        if (filePtr_ == NULL)
        {   
            // Open for read/write failed
            if ((errno == EROFS) || (errno == EACCES))
            {
                // Can we open the file read-only instead?
                if ((flags_ & (UNIT_RO | UNIT_ROABLE)) == 0)
                    return StatusCode::ReadOnlyNotAllowed;

                // Try to open the file read-only
                filePtr_ = sim_fopen (fileName_.c_str(), "rb");
                if (filePtr_ == NULL)
                    return StatusCode::OpenError;
                flags_ |= UNIT_RO;
                if (!CmdLineOptions::get().quiet)
                    std::cout << owningDevice_->name() << ": unit is read only\n";
            }
            else
            {
                // The file doesn't exist
                // Only allowed to open existing files?
                if (CmdLineOptions::get().openExistingFile)
                    return StatusCode::OpenError;

                // Open new file with the specified name
                filePtr_ = sim_fopen (fileName_.c_str(), "wb+");
                if (filePtr_ == NULL)
                    return StatusCode::OpenError;

                if (!CmdLineOptions::get().quiet)
                    std::cout << owningDevice_->name() << ": creating new file\n";
            }
        }
    }

    if (flags_ & UNIT_BUFABLE)
    {
        // aincr is the increment between device addresses, normally 1;
        // however, byte addressed devices with 16-bit words specify 2,
        // with 32-bit words 4.
        // See http://simh.trailing-edge.com/docs/simh.pdf
        // As for the PDP-11 the address increment is 1, the effective size
        // equals the defined capacity.
        // uint32 cap = ((uint32)uptr->capac) / dptr->aincr;  /* effective size */

        // The I/O must be buffered?
        if (flags_ & UNIT_MUSTBUF)
            // Allocate memory for buffered I/O. SZ_D hides a complex
            // calculation of the word (?) size. For the RLV12 this 
            // comes down to 16, so we allocate the buffer in u16's.
            // uptr->filebuf = calloc(cap, SZ_D(dptr));
            fileBuffer_ = new (std::nothrow) u16[capacity_]();

        if (fileBuffer_ == NULL)
            throw ("Allocating memory for buffered I/O failed");

        if (!CmdLineOptions::get().quiet)
            std::cout << owningDevice_->name() << ": buffering file in memory\n";
            
        hwmark_ = sim_fread (fileBuffer_, sizeof (u16), capacity_ , filePtr_);

        // Mark unit buffered
        flags_ |= UNIT_BUF;
    }
    flags_ |= UNIT_ATT;
    position_ = 0;
    return StatusCode::OK;
}
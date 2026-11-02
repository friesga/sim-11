#include "cartridge.h"
#include "cmdlineoptions/cmdlineoptions.h"

bool Cartridge::isAttached () const
{
    return (diskFileStream_ != nullptr);
}

// Return the size of the attached file in bytes
t_offset Cartridge::attachedFileSize () const
{
    return fio::fsize (diskFileStream_);
}
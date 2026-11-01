#include "cartridge.h"
#include "cmdlineoptions/cmdlineoptions.h"

StatusCode Cartridge::attachFile (std::string fileName, Geometry geometry,
    OpenMode attachMode)
{
    StatusCode statusCode;

    // Save the given geometry for this drive so it can be used to
    // calculate file positions.
    geometry_ = geometry;

    switch (attachMode)
    {
        case OpenMode::ReadWrite:
            statusCode = openReadWrite (fileName);
            break;

        case OpenMode::ReadOnly:
            statusCode = openReadOnly (fileName);
            break;

        case OpenMode::NewFile:
            statusCode = createFile (fileName);
            break;

        default:
            return StatusCode::ArgumentError;
    }

    return statusCode;
}

bool Cartridge::isAttached () const
{
    return (diskFileStream_ != nullptr);
}

// Return the size of the attached file in bytes
t_offset Cartridge::attachedFileSize () const
{
    return fio::fsize (diskFileStream_);
}
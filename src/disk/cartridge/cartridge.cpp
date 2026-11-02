#include "cartridge.h"

using std::unique_ptr;
using std::make_unique;
using std::move;

Cartridge::Cartridge (std::string fileName, Geometry geometry,
    OpenMode openMode)
{
    StatusCode statusCode;

    // Save the given geometry for this drive so it can be used to
    // calculate file positions.
    geometry_ = geometry;

    switch (openMode)
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
            statusCode = StatusCode::ArgumentError;
    }

    if (statusCode != StatusCode::Success)
        throw std::runtime_error ("Cartridge failed to attach file: " + fileName);
}

std::unique_ptr<Cartridge> Cartridge::create (std::string fileName,
    Geometry geometry, OpenMode openMode)
{
    try
    {
        // Create a new Cartridge object and return it as a unique_ptr. This detour
        // is required as make_unique has no access to the private Cartridge
        // constructor.
        unique_ptr<Cartridge> cartridge = unique_ptr<Cartridge> (
            new Cartridge (fileName, geometry, openMode));
        return move (cartridge);
    }
    catch (const std::runtime_error& e)
    {
        return nullptr;
    }
}
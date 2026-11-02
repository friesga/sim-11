#include "cartridge.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "logger/logger.h"
#include "fio/fio.h"

StatusCode Cartridge::openReadWrite (std::string fileName)
{
    diskFileStream_ = fio::fopen(fileName.c_str(), "rb+");

    return (diskFileStream_ != NULL) ? StatusCode::Success :
        StatusCode::OpenError;
}
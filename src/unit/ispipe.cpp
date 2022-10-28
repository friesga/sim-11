
#include "unit.h"

#include <string>
#include <filesystem>
#include <system_error>
// #include <sys/stat.h>

bool Unit::isPipe (std::string fileName)
{
    // struct stat info;
    // return !stat (fileName_.c_str(), &info) && (info.st_mode & _S_IFIFO);
    std::error_code ec;
    bool res = std::filesystem::is_fifo (fileName, ec);
    if (ec.value() != 0)
        return false;

    return res;
}
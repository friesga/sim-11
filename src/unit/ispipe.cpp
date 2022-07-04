
#include "unit.h"

#include <string>

bool Unit::isPipe (std::string fileName)
{
    struct stat info;

    return !stat (fileName_.c_str(), &info) && (info.st_mode & _S_IFIFO);
}
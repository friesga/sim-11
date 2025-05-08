#include "unit.h"

void DiskDrive::setWriteProtected (bool writeProtected)
{
    writeProtected_ = writeProtected;
}


bool DiskDrive::isWriteProtected () const
{
    return writeProtected_;
}
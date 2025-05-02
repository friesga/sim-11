#include "unit.h"

void Unit::setWriteProtected (bool writeProtected)
{
    writeProtected_ = writeProtected;
}


bool Unit::isWriteProtected () const
{
    return writeProtected_;
}
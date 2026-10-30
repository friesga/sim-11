#include "cartridge.h"

void Cartridge::setWriteProtected (bool writeProtected)
{
    writeProtected_ = writeProtected;
}


bool Cartridge::isWriteProtected () const
{
    return writeProtected_;
}
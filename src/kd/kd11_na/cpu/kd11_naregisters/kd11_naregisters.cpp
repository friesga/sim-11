#include "kd11_naregisters.h"

#include <string>

using std::string;

KD11_NARegisters::KD11_NARegisters ()
    :
    registers_ {}
{}

u16& KD11_NARegisters::operator[] (u16 registerNr)
{
    if (registerNr >= numRegisters)
        throw string ("Illegal register number");

    return registers_ [registerNr];
}

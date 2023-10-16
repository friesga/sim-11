#include "kd11_naregisters.h"

#include <string>
#include <algorithm>

using std::string;
using std::copy;
using std::begin;
using std::end;

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

// Convert the object to an array of eight u16's. As a C-style array cannot
// be returned from a function we use a pointer to a static array. This is ok
// as long as there is just one KDF11_ARegisters object and that's the case
// as there is one processor in the system.
KD11_NARegisters::operator registerArray ()
{
    static u16 registers[8];
    copy (begin (registers_), end (registers_), registers);
    return registers;
}

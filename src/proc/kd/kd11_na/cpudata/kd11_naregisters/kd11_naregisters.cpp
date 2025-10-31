#include "kd11_naregisters.h"

#include <string>
#include <algorithm>
#include <stdexcept>

using std::string;
using std::copy;
using std::begin;
using std::end;
using std::invalid_argument;

// The psw argument is ignored. It is required by the KDF11Registers'
// constructor but isn't used on the KD11-NA.
KD11_NARegisters::KD11_NARegisters (u16 const &psw)
    :
    registers_ {}
{}

u16& KD11_NARegisters::operator[] (u16 registerNr)
{
    if (registerNr >= numRegisters)
        throw invalid_argument ("Illegal register number");

    return registers_ [registerNr];
}

// Return the contents of the specified register in to the previous memory
// management mode. As the KDF11-NA has no memory management modes the current
// mode contents are returned.
u16& KD11_NARegisters::prevModeContents (u16 registerNr)
{
    return this->operator[](registerNr);
}

u16& KD11_NARegisters::contents (u16 registerNr, u16 mode)
{
    return this->operator[](registerNr);
}

// Write the given contents to the given register, using the previous
// memory management mode. As the KDF11-NA has no memory management modes the current
// mode contents are returned.
void KD11_NARegisters::writePrevMode (u16 registerNr, u16 contents)
{
    this->operator[](registerNr) = contents;
}

// Convert the object to an array of eight u16's. As a C-style array cannot
// be returned from a function we use a pointer to a static array. This is ok
// as long as there is just one KDF11Registers object and that's the case
// as there is one processor in the system.
KD11_NARegisters::operator registerArray ()
{
    static u16 registers[8];
    copy (begin (registers_), end (registers_), registers);
    return registers;
}

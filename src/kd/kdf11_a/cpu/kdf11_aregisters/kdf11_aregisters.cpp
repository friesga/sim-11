#include "kdf11_aregisters.h"

#include <string>
#include <algorithm>

using std::string;
using std::copy;
using std::begin;
using std::end;

KDF11_ARegisters::KDF11_ARegisters (u16 const& psw)
    :
    psw_ {psw},
    registers_ {0},
    R6_ {0}
{}

u16& KDF11_ARegisters::operator[] (u16 registerNr)
{
    if (registerNr >= numRegisters)
        throw string ("Illegal register number");

    if (registerNr == 6)
        return R6_ [memMgmtMode (psw_)];

    return registers_ [registerNr];
}

// Return the contents of the specified register in to the previous memory
// management mode.
u16& KDF11_ARegisters::prevModeContents (u16 registerNr)
{
    if (registerNr >= numRegisters)
        throw string ("Illegal register number");

    if (registerNr == 6)
        return R6_ [previousMemMgmtMode (psw_)];

    return registers_ [registerNr];
}

// Write the given contents to the given register, using the previous
// memory management mode
void KDF11_ARegisters::writePrevMode (u16 registerNr, u16 contents)
{
    if (registerNr >= numRegisters)
        throw string ("Illegal register number");

    if (registerNr == 6)
        R6_ [previousMemMgmtMode (psw_)] = contents;

    registers_ [registerNr] = contents;
}

// Convert the object to an array of eight u16's. As a C-style array cannot
// be returned from a function we use a pointer to a static array. This is ok
// as long as there is just one KDF11_ARegisters object and that's the case
// as there is one processor in the system.
KDF11_ARegisters::operator registerArray ()
{
    static u16 registers[8];

    // First copy the registers_ array and then overwrite R6 with the correct
    // value.
    copy (begin (registers_), end (registers_), registers);
    registers [6] = R6_ [memMgmtMode (psw_)];
    return registers;
}

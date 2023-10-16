#include "kdf11_aregisters.h"

#include <string>

using std::string;

KDF11_ARegisters::KDF11_ARegisters (u16& psw)
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

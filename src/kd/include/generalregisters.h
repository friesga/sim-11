#ifndef _GENERALREGISTERS_H_
#define _GENERALREGISTERS_H_

#include "types.h"

// This class defines the interface for access to the general registers
// in the CPU. This allows different implementations for the different 
// register sets of the PDP-11 processor family. As a bonus the registerNr
// can be checked against the number of registers.
//
class GeneralRegisters
{
public:
    virtual u16& operator[] (u16 registerNr) = 0;
};

#endif // _GENERALREGISTERS_H_
#ifndef _GENERALREGISTERS_H_
#define _GENERALREGISTERS_H_

#include "types.h"

// This class defines the interface for access to the general registers
// in the CPU. This allows different implementations for the different 
// register sets of the PDP-11 processor family. As a bonus the registerNr
// can be checked against the number of registers.
//
// The u16* conversion operator is used to convert the registers to an
// array which can then be passed to the Trace::cpuStep(). This ensures
// the actually used stack pointer is written to the trace file.
class GeneralRegisters
{
public:
    using registerArray = u16*;

    virtual u16& operator[] (u16 registerNr) = 0;
    virtual u16& prevModeContents (u16 registerNr) = 0;
    virtual u16& contents (u16 registerNr, u16 mode) = 0;
    virtual void writePrevMode (u16 registerNr, u16 contents) = 0;
    virtual operator registerArray() = 0;
};

#endif // _GENERALREGISTERS_H_
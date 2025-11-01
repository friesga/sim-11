#include "registeroperandlocation.h"


RegisterOperandLocation::RegisterOperandLocation (CpuData* cpuData, u8 registerNumber)
    :
    location_ {registerNumber},
    cpuData_ {cpuData}
{}

// Return whether or not the OperandLocation points to a valid address
bool RegisterOperandLocation::isValid ()
{
    return true;
}

// Return, depending on (the type of) the value held in the location, either
// the register number or the memory address held in the location. The type
// of the value can be determined by means of the isA<> function.
RegisterOperandLocation::operator u16 ()
{
    return location_;    
}


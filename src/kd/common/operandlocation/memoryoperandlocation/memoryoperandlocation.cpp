#include "memoryoperandlocation.h"

MemoryOperandLocation::MemoryOperandLocation (CpuData *cpu, MMU* mmu, CondData<u16> memoryAddress)
    :
    location_ {memoryAddress},
    cpu_ {cpu},
    mmu_ {mmu}
{}

// Return whether or not the OperandLocation points to a valid address
bool MemoryOperandLocation::isValid ()
{
    return location_.hasValue ();
}

// Return, depending on (the type of) the value held in the location, either
// the register number or the memory address held in the location. The type
// of the value can be determined by means of the isA<> function.
MemoryOperandLocation::operator u16 ()
{
    return location_.valueOr (0);
}


#ifndef _WITHFACTORY_H_
#define _WITHFACTORY_H_

#include "kd/common/lsi11instruction/lsi11instruction.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

// This class adds a factory function to an existing class by providing it
// with a static create() function which creates an object of the specified
// type.
template <typename T>
class WithFactory
{
public:
    static unique_ptr<LSI11Instruction> create (CpuData* cpuData,
        CpuControl* cpuControl, MMU* mmu, u16 instruction)
    {
        return make_unique<T> (cpuData, cpuControl, mmu, instruction);
    }
};


#endif // _WITHFACTORY_H_
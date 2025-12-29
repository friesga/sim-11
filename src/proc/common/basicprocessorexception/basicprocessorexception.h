#ifndef _BASICPROCESSOREXCEPTION_H_
#define _BASICPROCESSOREXCEPTION_H_

#include "proc/common/execution_engine/execution_engine.h"
#include "proc/include/cpucontrol.h"

// This class implements basic processor exception handling without stack
// overflow detection and handling.
//
class BasicProcessorException
{
public:
    BasicProcessorException (Bus* bus, CpuData* cpuData, MMU* mmu,
        Interface::ExecutionEngine& executionEngine);
    void serviceTrap ();
    void serviceInterrupt ();

private:
    Bus* bus_;
    CpuData* cpuData_;
    MMU* mmu_;
    Interface::ExecutionEngine& executionEngine_;

    void swapPcPSW (u16 vectorAddress);
    bool fetchFromVector (u16 address, u16* dest);
    bool fetchFromVector (u16 address, function<void (u16)> lambda);
};

#endif // _BASICPROCESSOREXCEPTION_H_
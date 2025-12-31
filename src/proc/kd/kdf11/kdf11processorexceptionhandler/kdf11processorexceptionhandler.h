#ifndef _KDF11PROCESSOREXCEPTIONHANDLER_H_
#define _KDF11PROCESSOREXCEPTIONHANDLER_H_

#include "bus/include/bus.h"
#include "proc/kd/include/cpudata.h"
#include "proc/kd/include/mmu.h"
#include "proc/include/cpucontrol.h"

// Functions to service a trap for processors implementing stack overflow
// detection and handling.
//
// The interface to the ExecutionEngine isn't used in this class.
//
class KDF11ProcessorExceptionHandler
{
public:
    KDF11ProcessorExceptionHandler (Bus* bus, CpuData* cpuData,
        CpuControl* cpuControl, MMU* mmu);
    void serviceTrap ();
    void serviceInterrupt ();

private:
    Bus* bus_;
    CpuData* cpuData_;
    MMU* mmu_;

    void swapPcPSW (u16 vectorAddress);
    bool fetchFromVector (u16 address, u16* dest);
};

#endif // _KDF11PROCESSOREXCEPTIONHANDLER_H_
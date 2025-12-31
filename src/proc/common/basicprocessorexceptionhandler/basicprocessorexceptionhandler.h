#ifndef _BASICPROCESSOREXCEPTIONHANDLER_H_
#define _BASICPROCESSOREXCEPTIONHANDLER_H_

#include "bus/include/bus.h"
#include "proc/kd/include/cpudata.h"
#include "proc/kd/include/mmu.h"
#include "proc/include/cpucontrol.h"

// This class implements basic processor exception handling without stack
// overflow detection and handling.
//
class BasicProcessorExceptionHandler
{
public:
    BasicProcessorExceptionHandler (Bus* bus, CpuData* cpuData,
        CpuControl* cpuControl, MMU* mmu);
    void serviceTrap ();
    void serviceInterrupt ();

private:
    Bus* bus_;
    CpuData* cpuData_;
    CpuControl* cpuControl_;
    MMU* mmu_;

    void swapPcPSW (u16 vectorAddress);
    bool fetchFromVector (u16 address, u16* dest);
    bool fetchFromVector (u16 address, function<void (u16)> lambda);
};

#endif // _BASICPROCESSOREXCEPTIONHANDLER_H_
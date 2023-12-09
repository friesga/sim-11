#ifndef _DUMMYCPU_H_
#define _DUMMYCPU_H_

#include "dummycpucontrol.h"
#include "dummycpudata.h"
#include "dummymmu/dummymmu.h"

// Dummy implementation of a CPU to be used in unit tests.
class DummyCpu
{
public:
    // Give unit tests access to the CPU, CpuData and the MMU.
    constexpr CpuControl* cpu () { return &cpuControl_; };
    constexpr CpuData* cpuData () { return &cpuData_; };
    constexpr MMU* mmu () { return &dummyMMU_; };

private:
    DummyCpuControl cpuControl_;
    DummyCpuData cpuData_;
    DummyMMU dummyMMU_;
};

#endif // _DUMMYCPU_H_
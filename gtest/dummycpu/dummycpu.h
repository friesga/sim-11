#ifndef _DUMMYCPU_H_
#define _DUMMYCPU_H_

#include "bus/qbus/qbus.h"
#include "dummycpucontroller.h"
#include "dummycpudata.h"
#include "dummymmu/dummymmu.h"
#include "proc/common/datapaths/datapaths.h"

// Dummy implementation of a CPU to be used in unit tests.
class DummyCpu
{
public:
    // Give unit tests access to the CPU, CpuData and the MMU.
    constexpr Interfaces::CpuController* cpuControl () { return &cpuControl_; };
    constexpr CpuData* cpuData () { return &cpuData_; };
    constexpr DataPaths* dataPaths () { return &dataPaths_; };

private:
    Qbus bus_ {};
    DummyCpuControl cpuControl_;
    DummyCpuData cpuData_;
    DummyMMU dummyMMU_;
    DataPaths dataPaths_ {&bus_, &cpuData_, &dummyMMU_};
};

#endif // _DUMMYCPU_H_
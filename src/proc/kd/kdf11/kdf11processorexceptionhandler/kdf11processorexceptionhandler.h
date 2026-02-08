#ifndef _KDF11PROCESSOREXCEPTIONHANDLER_H_
#define _KDF11PROCESSOREXCEPTIONHANDLER_H_

#include "bus/include/bus.h"
#include "proc/include/cpudata.h"
#include "proc/common/datapaths/datapaths.h"
#include "proc/include/cpucontroller.h"

// Functions to service a trap for processors implementing stack overflow
// detection and handling.
//
// The interface to the ExecutionEngine isn't used in this class.
//
class KDF11ProcessorExceptionHandler
{
public:
    KDF11ProcessorExceptionHandler (Bus* bus, CpuData* cpuData,
        Interfaces::CpuController* cpuController, DataPaths* dataPaths);
    void serviceTrap ();
    void serviceInterrupt ();

private:
    Bus* bus_;
    CpuData* cpuData_;
    Interfaces::CpuController* cpuController_;
    DataPaths* dataPaths_;

    void swapPcPSW (u16 vectorAddress);
    bool fetchFromVector (u16 address, u16* dest);
};

#endif // _KDF11PROCESSOREXCEPTIONHANDLER_H_
#ifndef _KDF11PROCESSOREXCEPTION_H_
#define _KDF11PROCESSOREXCEPTION_H_

#include "proc/common/execution_engine/execution_engine.h"
#include "proc/include/cpucontrol.h"

class KDF11ProcessorException
{
public:
    KDF11ProcessorException (Bus* bus, CpuData* cpuData, MMU* mmu);
    void serviceTrap ();
    void serviceInterrupt ();

private:
    Bus* bus_;
    CpuData* cpuData_;
    MMU* mmu_;

    void swapPcPSW (u16 vectorAddress);
    bool fetchFromVector (u16 address, u16* dest);
};

#endif // _KDF11PROCESSOREXCEPTION_H_
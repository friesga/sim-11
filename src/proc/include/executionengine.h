#ifndef _INTERFACE_EXECUTIONENGINE_H_
#define _INTERFACE_EXECUTIONENGINE_H_

#include "proc/include/cpucontrol.h"

namespace Interface
{
    class ExecutionEngine
    {
    public:
        virtual void cpuReset () = 0;
        virtual void busReset () = 0;
        virtual void halt (CpuControl::HaltReason haltreason) = 0;
        virtual void wait () = 0;
        virtual void start (u16) = 0;
        virtual void proceed () = 0;
        virtual CpuControl::HaltReason haltReason () = 0;
        virtual CpuControl::CpuRunState execute () = 0;
    };
}


#endif // _INTERFACE_EXECUTIONENGINE_H_
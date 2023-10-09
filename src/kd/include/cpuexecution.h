#ifndef _CPUEXECUTION_H_
#define _CPUEXECUTION_H_

// Thi class defines the function(s) to execute instructions by a CPU
class CpuExecution
{
public:
    // The step has to return true if a following instruction can be execued,
    // false otherwise. In the latter case a HALT instruction was executed.
    virtual bool step () = 0;
};

#endif // _CPUEXECUTION_H_
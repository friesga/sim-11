#ifndef _BASICSTACKOVERFLOWPOLICY_H_
#define _BASICSTACKOVERFLOWPOLICY_H_

class BasicStackOverflowPolicy
{
public:
    // Check if a stack overflow has occurred, i.e. the kernel stack pointer has
    // been decremented below the stack limit.
    // On a double bus error a new stack will be set up at locations 2 and 0. This
    // should not result in a stack overflow trap.
    inline bool stackOverflow () const
    {
        return inKernelMode () &&
            cpuData ()->registers() [6] > 0 && cpuData ()->register ()[6] < stackLimit;
    }

private:
    constexpr bool inKernelMode ()
    {
        return cpuData ()->psw_.currentMode () == PSW::Mode::Kernel;
    }
};

#endif // _BASICSTACKOVERFLOWPOLICY_H_
#ifndef _STACK_H_
#define _STACK_H_

#include "proc/include/cpudata.h"
#include "proc/common/datapaths/datapaths.h"

// The PDP-11 makes heavy use of stacks, for which it has good support via
// the auto-increment and auto-decrement addressing modes. However, there are
// a number of sometimes poorly-documented details of their operation, which
// ward off potential issues with the main stack (pointed to by R6, the main
// Stack Pointer).
//
// - Byte operations (push and pop) on the R6 stack always adjust the SP by 2,
//   not 1; this to ensure that it always contains a word address, should an
//   unexpected interrupt or trap require it to be used to save the old PS
//   and PC.
// - On CPU models intended for use in time-sharing systems, which support
//   User and Kernel modes, there are separate R6's for the two modes, so that
//   the operating system can use the latter one for interrupt and trap
//   handling, and thereby guarantee that the user cannot derange R6's
//   contents before it needs to be used.
// - Almost all models have some form of stack address limitation, which
//   causes a trap when the stack overflows (or tries to). The details vary
//   from model to model; see below for details.
// - When an interrupt or trap occurs, if R6 does contain either an odd
//   (i.e.non-word) address, or an address which does not point to functional
//   main memory, on many models R6 is then bashed to contain 4, after which
//   the old PC and PS are saved in 0 and 2 (respectively); after that, the
//   CPU takes an odd-address/non-existent-memory trap. (KDF11 CPUs ignore odd
//   stack addresses, and proceed as if the bottom address bit in their SP is
//   0. Apparently LSI-11 CPUs also ignores odd stack addresses.)
//
// On the KA11 CPU (PDP-11/20), KD11-B CPU (PDP-11/05), KD11-D CPU (PDP-11/04),
// KD11-E CPU (PDP-11/34), KD11-Z CPU (PDP-11/44), and LSI-11 CPUs, a fault
// during a trap will halt the CPU.
//
// Source: https://gunkies.org/wiki/PDP-11_stacks
//
template <typename TStackOverflowPolicy>
class Stack : public TStackOverflowPolicy
{
public:
    Stack (CpuData* cpuData, DataPaths* dataPaths);
    bool pushWord (u16 value);
    bool popWord (u16* destination);

    // ToDo: To be merged with pushWord()
    bool stackOverflow () const;

private:
    CpuData* cpuData_;
    DataPaths* dataPaths_;
};

template <typename TStackOverflowPolicy>
Stack<TStackOverflowPolicy>::Stack (CpuData* cpuData, DataPaths* dataPaths)
    :
    cpuData_ {cpuData},
    dataPaths_ {dataPaths}
{}

// Push the given value on the processor stack
template <typename TStackOverflowPolicy>
bool Stack<TStackOverflowPolicy>::pushWord (u16 value)
{
    cpuData_->registers ()[6] -= 2;
    return dataPaths_->putWord (cpuData_->registers ()[6], value);
}

// Pop a word from the processor stack returning true if this succeeds
// or false when a bus error occurs.
template <typename TStackOverflowPolicy>
bool Stack<TStackOverflowPolicy>::popWord (u16* destination)
{
    CondData<u16> tmpValue = dataPaths_->fetchWord (cpuData_->registers ()[6]);
    *destination = tmpValue.valueOr (0);
    cpuData_->registers ()[6] += 2;
    if (!tmpValue.hasValue ())
        return false;
    return true;
}

template <typename TStackOverflowPolicy>
bool Stack<TStackOverflowPolicy>::stackOverflow () const
{
    return static_cast <const TStackOverflowPolicy&> (this->stackOverflow ());
}

#endif // _STACK_H_
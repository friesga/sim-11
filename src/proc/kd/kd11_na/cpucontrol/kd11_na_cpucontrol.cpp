#include "kd11_na_cpucontrol.h"
#include "trace/trace.h"
#include "proc/kd/kd11_na/executor/executor.h"

#include <functional>
#include <chrono>
#include <variant>

using std::visit;
using std::chrono::microseconds;
using std::runtime_error;

// Constructor
template <typename TExecutor, typename TCalculator, typename THaltMode>
KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::KD11_NA_CpuControl (Bus* bus, CpuData* cpuData, MMU* mmu)
    :
    bus_ {bus},
    mmu_ {mmu},
    cpuData_ {cpuData}
{}

// Reset the processor
// 
// Clear the registers and the PSW
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::cpuReset ()
{
    engine_.cpuReset ();
}

// Reset (the devices on) the bus by setting the INIT signal
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::busReset ()
{
    engine_.busReset ();
}

// Halt the processor
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::halt ()
{
    engine_.halt ();
}

// Wait for an interrupt
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::wait ()
{
    engine_.wait ();
}

// Start the processor at the given address
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::start (u16 address)
{
    engine_.start (address);
}

// Continue execution at the current PC
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::proceed ()
{
    engine_.proceed ();
}

// The HaltMode is not implemented in the KD11-NA. inHaltMode() returns
// false to prevent the CPU being halted by an unsupported feature.
// 
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::setHaltMode (bool haltMode)
{
    haltMode_.setHaltMode (haltMode);
}

template <typename TExecutor, typename TCalculator, typename THaltMode>
bool KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::inHaltMode ()
{
    return haltMode_.inHaltMode ();
}

template <typename TExecutor, typename Calculator, typename THaltMode>
CpuControl::HaltReason KD11_NA_CpuControl<TExecutor, Calculator, THaltMode>::haltReason ()
{
    return engine_.haltReason ();
}

template <typename TExecutor, typename TCalculator, typename THaltMode>
CpuControl::CpuRunState KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::execute ()
{
    return engine_.execute ();
}


// Explicit template instantation requests as the class function members are
// defined outside of the class header file.
template class KD11_NA_CpuControl<KD11_NA_Executor, KD11_NA_Calculate, PseudoHaltMode>;
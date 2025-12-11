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
    cpuData_ {cpuData},
    runState {CpuRunState::HALT},
    decoder {},
    haltReason_ {HaltReason::HaltInstruction},
    traceFlag_ {false}
{
    bus_->SRUN ().set (false);
}

// Reset the processor
// 
// Clear the registers and the PSW
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::cpuReset ()
{
    // Initialize the registers except for the PC
    for (u16 regNr = 0; regNr <= 6; ++regNr)
        cpuData_->registers ()[regNr] = 0;

    cpuData_->psw ().set (PSW::ProtectionMode::Reset, 0);
}

// Reset (the devices on) the bus by setting the INIT signal
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::busReset ()
{
    bus_->BINIT().cycle ();
}

// Halt the processor
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::halt ()
{
    runState = CpuRunState::HALT;
    haltReason_ = HaltReason::HaltInstruction;
    bus_->SRUN().set (false);
    trace.cpuEvent (CpuEventRecordType::CPU_HALT, cpuData_->registers ()[7]);
}

// Wait for an interrupt
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::wait ()
{
    trace.cpuEvent (CpuEventRecordType::CPU_WAIT, cpuData_->registers ()[7]);
    runState = CpuRunState::WAIT;
}

// Start the processor at the given address
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::start (u16 address)
{
    cpuData_->registers ()[7] = address;
    runState = CpuRunState::RUN;
    bus_->SRUN().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_G, address);
}

// Continue execution at the current PC
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::proceed ()
{
    runState = CpuRunState::RUN;
    bus_->SRUN().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, cpuData_->registers ()[7]);
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
constexpr CpuControl::HaltReason KD11_NA_CpuControl<TExecutor, Calculator, THaltMode>::haltReason ()
{
    return haltReason_;
}

template <typename TExecutor, typename TCalculator, typename THaltMode>
CpuControl::CpuRunState KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::execute ()
{
    // If there is a pending bus interrupt that can be executed, process
    // that interrupt first, else execute the next instruction
    // Interrupts are only processed if their priority is higher than the
    // current CPU priority. (The LSI-11 has just two priority levels,
    // zero and BR4.) Note that the numerical value of the InterruptPriority enum
    // is used as bus request level. Traps in HALT mode are ignored.
    if (cpuData_->trapPending ())
    {
        serviceTrap ();
        traceFlag_ =  (cpuData_->psw ().traceBitSet ()) ? true : false;
    }

    if (bus_->intrptReqAvailable () && bus_->intrptPriority () > cpuPriority ())
        serviceInterrupt ();

    if (trace.isActive ())
        traceStep ();

    execInstr ();

    // Instructions leave the run state unchanged except for the WAIT and HALT
    // instruction which sets the state to respectively WAIT and HALT.
    return runState;
}

// Execute one instruction
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::execInstr ()
{
    // Get next instruction to execute and move PC forward
    CondData<u16> instructionWord = mmu_->fetchWord (cpuData_->registers ()[7]);
    if (!instructionWord.hasValue ())
    {
        trace.bus (BusRecordType::ReadFail, cpuData_->registers ()[7], 0);
        cpuData_->setTrap (CpuData::TrapType::BusError);
        return;
    }
    cpuData_->registers ()[7] += 2;

    Instruction instr =
        decoder.decode (instructionWord);

    // The instruction time is defined in microseconds with an accuracy of
    // nanoseconds. Convert the time in microseconds to the 64-bits integer
    // number of nanoseconds.
    double instrTime = visit (calculator_, instr);
    SimulatorClock::forwardClock
    (
        SimulatorClock::duration (static_cast<uint64_t> (instrTime * 1000))
    );

    // Execute the next instruction. The function returns true if the
    // instruction was completed and false if it was aborted due to an error
    // condition. In that case a trap has been set. Note however that trap
    // instructions set a trap and return true. 
    visit (executor_, instr);

    // If the trace flag is set, the next instruction has to result in a trace
    // trap, unless the instruction resulted in another trap, depending on
    // the trap priorities.
    if (traceFlag_)
        cpuData_->setTrap (CpuData::TrapType::BreakpointTrap);

    // Trace Trap is enabled by bit 4 of the PSW and causes processor traps at
    // the end of instruction execution. The instruction-that is executed
    // after the instruction that set the T-bit will proceed to completion and
    // then trap through the trap vector at address 14.
    // LSI-11/PDP-11/03 Processor Handbook pag. 114.
    traceFlag_ =  (cpuData_->psw ().traceBitSet ()) ? true : false;
}

template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::serviceTrap ()
{
    // The enum trap_ is converted to the u16 vector address
    // Swap the PC and PSW with new values from the trap vector to process.
    // If this fails the processor will be put in the HALT state.
    swapPcPSW (cpuData_->trapVector ());
    cpuData_->clearTrap ();
}

template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::serviceInterrupt ()
{
    InterruptRequest intrptReq;

    if (bus_->getIntrptReq (intrptReq))
        // Swap the PC and PSW with new values from the trap vector to process.
        // If this fails the processor will be put in the HALT state.
        swapPcPSW (intrptReq.vector ());
}

template <typename TExecutor, typename TCalculator, typename THaltMode>
u8 KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::cpuPriority ()
{
    return cpuData_->psw ().priorityLevel ();
}

// Fetch PC and PSW from the given vector address. If this fails the
// processor will halt anyway.
template <typename TExecutor, typename TCalculator, typename THaltMode>
bool KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::fetchFromVector (u16 address, u16* dest)
{
    CondData<u16> tmpValue = mmu_->fetchWord (address);
    *dest = tmpValue.valueOr (0);
    return tmpValue.hasValue ();
}

template <typename TExecutor, typename TCalculator, typename THaltMode>
bool KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::fetchFromVector (u16 address, function<void (u16)> lambda)
{
    CondData<u16> tmpValue = mmu_->fetchWord (address);
    lambda (tmpValue.valueOr (0));
    return tmpValue.hasValue ();
}

// Swap the PC and PSW with new values from the given vector
template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::swapPcPSW (u16 vectorAddress)
{
    trace.cpuEvent (CpuEventRecordType::CPU_TRAP, vectorAddress);

    // Save PC and PSW on the stack. Adressing the stack could result in a
    // bus time out. In that case the CPU is halted.
    if (!mmu_->pushWord (cpuData_->psw ()) || !mmu_->pushWord (cpuData_->registers ()[7]))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, cpuData_->registers ()[6]);
        // ToDo: All interrupts should be cleared?
        cpuData_->clearTrap ();
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::DoubleBusError;
        bus_->SRUN ().set (false);
        return;
    }

    // Read new PC and PSW from the trap vector. These read's could also
    // result in a bus time out.
    if (!fetchFromVector (vectorAddress, &cpuData_->registers ()[7]) ||
        !fetchFromVector (vectorAddress + 2, [this] (u16 value)
            {cpuData_->psw ().set (PSW::ProtectionMode::Trap, value); }))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, vectorAddress);
        cpuData_->clearTrap ();
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::BusErrorOnIntrptVector;
        bus_->SRUN ().set (false);
        return;
    }
}

template <typename TExecutor, typename TCalculator, typename THaltMode>
void KD11_NA_CpuControl<TExecutor, TCalculator, THaltMode>::traceStep ()
{
    trace.setIgnoreBus ();
    u16 code[3];
    // The read of register_[7]+2 and  register_[7]+4 may access an invalid address as
    // the instruction isn't decoded at this point. Therefore use the bus
    // read function instead of fetchWord(). The latter will generate a
    // bus error trap on access of an invalid address.
    code[0] = mmu_->readWithoutTrap (cpuData_->registers ()[7] + 0).valueOr (0);
    code[1] = mmu_->readWithoutTrap (cpuData_->registers ()[7] + 2).valueOr (0);
    code[2] = mmu_->readWithoutTrap (cpuData_->registers ()[7] + 4).valueOr (0);
    trace.cpuStep (cpuData_->registers (), cpuData_->psw (), code);
    trace.clearIgnoreBus ();
}

// Explicit template instantation requests as the class function members are
// defined outside of the class header file.
template class KD11_NA_CpuControl<KD11_NA_Executor, KD11_NA_Calculate, PseudoHaltMode>;
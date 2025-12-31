#include "composite_cpucontroller.h"
#include "trace/trace.h"
#include "chrono/simulatorclock/simulatorclock.h"
#include "float/float.h"
#include "bitmask.h"
#include "proc/kd/kd11_na/executor/executor.h"
#include "proc/kd/kd11_na/calculate/calculate.h"
#include "proc/common/pseudo_haltmode/pseudo_haltmode.h"
#include "proc/common/basicprocessorexceptionhandler/basicprocessorexceptionhandler.h"
#include "proc/kd/kdf11/executor/executor.h"
#include "proc/kd/kdf11/calculate/calculate.h"
#include "proc/kd/kdf11/haltmode/haltmode.h"
#include "proc/kd/kdf11/kdf11processorexception/kdf11processorexception.h"

#include <functional>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <iostream>

using std::unique_ptr;
using std::make_unique;
using std::runtime_error;
using std::cout;

using namespace std::chrono;

// Constructor
template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::CompositeCpuController (Bus* bus, CpuData* cpuData, MMU* mmu)
    :
    bus_ {bus},
    mmu_ {mmu},
    cpuData_ {cpuData},
    runState_ {CpuControl::CpuRunState::HALT},
    haltReason_ {CpuControl::HaltReason::HaltInstruction},
    traceFlag_ {false}
{
    executor_ = make_unique<TExecutor> (cpuData, this, mmu);
    calculator_ = make_unique<TCalculator> ();
    haltMode_ = make_unique<THaltMode> ();

    bus_->SRUN ().set (false);
}

// Reset the processor
// 
// Clear the registers and the PSW
template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
void CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::cpuReset ()
{
    // Initialize the registers except for the PC
    for (u16 regNr = 0; regNr <= 6; ++regNr)
        cpuData_->registers ()[regNr] = 0;

    cpuData_->psw ().set (PSW::ProtectionMode::Reset, 0);
}

// Reset (the devices on) the bus by setting the INIT signal and reset
// the KTF11-A.
template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
void CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::busReset ()
{
    bus_->BINIT ().cycle ();
    mmu_->reset ();
}

// Halt the processor
template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
void CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::halt (CpuControl::HaltReason reason)
{
    runState_ = CpuControl::CpuRunState::HALT;
    haltReason_ = reason;
    bus_->SRUN ().set (false);
    trace.cpuEvent (CpuEventRecordType::CPU_HALT, cpuData_->registers ()[7]);
}

template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
void CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::wait ()
{
    trace.cpuEvent (CpuEventRecordType::CPU_WAIT, cpuData_->registers ()[7]);
    runState_ = CpuControl::CpuRunState::WAIT;
}

// Start the processor at the given address
template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
void CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::start (u16 address)
{
    cpuData_->registers ()[7] = address;
    runState_ = CpuControl::CpuRunState::RUN;
    bus_->SRUN ().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_G, address);
}

// Continue execution at the current PC
template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
void CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::proceed ()
{
    runState_ = CpuControl::CpuRunState::RUN;
    bus_->SRUN ().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, cpuData_->registers ()[7]);
}

template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
void CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::setHaltMode (bool haltMode)
{
    haltMode_->setHaltMode (haltMode);
}

template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
bool CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::inHaltMode ()
{
    return haltMode_->inHaltMode ();
}

template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
constexpr CpuControl::HaltReason CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::haltReason ()
{
    return haltReason_;
}

// Execute an instruction. This mainly comprises three actions:
// 1. Execution of the instruction,
// 2. Handle the trace bit,
// 3. Handling of traps and interrupts that might have arisen during execution
//    of the instruction, either a trap as a result of an instruction, or an
//    interrupt requested by a bus device.
// 
// When the CPU is in the MachineState Running state, the CPU always is in of
// the following sub states:
// - HALT: the CPU is halted and cannot execute the step,
// - RUN: the CPU will execute the next instruction,
// - WAIT: the CPU is running but is still waiting for an interrupt,
// 
// The HALT and WAIT state will cause a transition in the Machine state
// machine.
// 
// As the power-up mode can be set to trap to the vector at address 024, the
// presence of traps is checked before an instruction is executed.
//
// The normal instruction flow can be interrupted by the setting of the BHALT
// or RESET bus signal. These signals are handled in their corresponding 
// receivers which then call a CpuControl control function.
//
// This function returns the new CPU state.
//
template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
CpuControl::CpuRunState CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::execute ()
{
    // If there is a pending bus interrupt that can be executed, process
    // that interrupt first, else execute the next instruction
    // Interrupts are only processed if their priority is higher than the
    // current CPU priority. (The LSI-11 has just two priority levels,
    // zero and BR4.) Note that the numerical value of the InterruptPriority enum
    // is used as bus request level. Traps in HALT mode are ignored.
    if (cpuData_->trapPending ())
    {
        processorException_.serviceTrap ();
        traceFlag_ =  (cpuData_->psw ().traceBitSet ()) ? true : false;
    }

    if (bus_->intrptReqAvailable () && bus_->intrptPriority () > cpuPriority ())
        processorException_.serviceInterrupt ();

    if (trace.isActive ())
        traceStep ();

    execInstr ();

    // Instructions leave the run state unchanged except for the WAIT and HALT
    // instruction which sets the state to respectively WAIT and HALT.
    return runState_;
}

// Execute one instruction
template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
void CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::execInstr ()
{
    // Get next instruction to execute and move PC forward
    CondData<u16> instructionWord = mmu_->fetchWord (cpuData_->registers ()[7]);
    if (!instructionWord.hasValue ())
    {
        trace.bus (BusRecordType::ReadFail, cpuData_->registers ()[7], 0);
        cpuData_->setTrap (CpuData::TrapType::BusError);
        return;
    }

    // During each instruction fetch SR2 is loaded with the 16-bit virtual
    // address (VA) but is not updated if the instruction fetch fails.
    mmu_->setVirtualPC (cpuData_->registers ()[7]);

    cpuData_->registers ()[7] += 2;

    Instruction instr =
        decoder_.decode (instructionWord);

    // The instruction time is defined in microseconds with an accuracy of
    // nanoseconds. Convert the time in microseconds to the 64-bits integer
    // number of nanoseconds.
    double instrTime = visit (*calculator_, instr);
    SimulatorClock::forwardClock
    (
        SimulatorClock::duration (static_cast<uint64_t> (instrTime * 1000))
    );

    // Execute the next instruction. The function returns true if the
    // instruction was completed and false if it was aborted due to an error
    // condition. In that case a trap has been set. Note however that trap
    // instructions set a trap and return true. 
    visit (*executor_, instr);

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

template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
u8 CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::cpuPriority ()
{
    return cpuData_->psw ().priorityLevel ();
}

template <isExecutor TExecutor, typename TCalculator, isHaltMode THaltMode,
    isProcessorException TProcessorException>
void CompositeCpuController<TExecutor, TCalculator, THaltMode,
    TProcessorException>::traceStep ()
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

// Explicit template instantiation to be able to define the methods in
// a separate .cpp file.
template class CompositeCpuController<KD11_NA_Executor, KD11_NA_Calculate,
    PseudoHaltMode, BasicProcessorExceptionHandler>;
template class CompositeCpuController<KDF11_Executor, KDF11_Calculate,
    KDF11_HaltMode, KDF11ProcessorException>;

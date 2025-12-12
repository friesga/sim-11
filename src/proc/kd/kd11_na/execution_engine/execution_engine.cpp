#include "execution_engine.h"
#include "proc/kd/common/instructions/instructions.h"

#include <variant>

using std::visit;

KD11_NA_ExecutionEngine::KD11_NA_ExecutionEngine (Bus* bus, CpuData* cpuData, MMU* mmu,
    KD11_NA_Executor* kd11_na_executor, KD11_NA_Calculate* kd11_na_calculator)
    :
    bus_ {bus},
    mmu_ {mmu},
    cpuData_ {cpuData},
    executor_ {kd11_na_executor},
    calculator_ {kd11_na_calculator},
    runState_ {CpuControl::CpuRunState::HALT},
    haltReason_ {CpuControl::HaltReason::HaltInstruction},
    traceFlag_ {false}
{
    bus_->SRUN ().set (false);
}

// Reset the processor
// 
// Clear the registers and the PSW
void KD11_NA_ExecutionEngine::cpuReset ()
{
    // Initialize the registers except for the PC
    for (u16 regNr = 0; regNr <= 6; ++regNr)
        cpuData_->registers ()[regNr] = 0;

    cpuData_->psw ().set (PSW::ProtectionMode::Reset, 0);
}

// Reset (the devices on) the bus by setting the INIT signal
void KD11_NA_ExecutionEngine::busReset ()
{
    bus_->BINIT ().cycle ();
}

// Halt the processor
void KD11_NA_ExecutionEngine::halt ()
{
    runState_ = CpuControl::CpuRunState::HALT;
    haltReason_ = CpuControl::HaltReason::HaltInstruction;
    bus_->SRUN ().set (false);
    trace.cpuEvent (CpuEventRecordType::CPU_HALT, cpuData_->registers ()[7]);
}

// Wait for an interrupt
void KD11_NA_ExecutionEngine::wait ()
{
    trace.cpuEvent (CpuEventRecordType::CPU_WAIT, cpuData_->registers ()[7]);
    runState_ = CpuControl::CpuRunState::WAIT;
}

// Start the processor at the given address
void KD11_NA_ExecutionEngine::start (u16 address)
{
    cpuData_->registers ()[7] = address;
    runState_ = CpuControl::CpuRunState::RUN;
    bus_->SRUN ().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_G, address);
}

// Continue execution at the current PC
void KD11_NA_ExecutionEngine::proceed ()
{
    runState_ = CpuControl::CpuRunState::RUN;
    bus_->SRUN ().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, cpuData_->registers ()[7]);
}

CpuControl::HaltReason KD11_NA_ExecutionEngine::haltReason ()
{
    return haltReason_;
}

CpuControl::CpuRunState KD11_NA_ExecutionEngine::execute ()
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
    return runState_;
}

// Execute one instruction
void KD11_NA_ExecutionEngine::execInstr ()
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

void KD11_NA_ExecutionEngine::serviceTrap ()
{
    // The enum trap_ is converted to the u16 vector address
    // Swap the PC and PSW with new values from the trap vector to process.
    // If this fails the processor will be put in the HALT state.
    swapPcPSW (cpuData_->trapVector ());
    cpuData_->clearTrap ();
}

void KD11_NA_ExecutionEngine::serviceInterrupt ()
{
    InterruptRequest intrptReq;

    if (bus_->getIntrptReq (intrptReq))
        // Swap the PC and PSW with new values from the trap vector to process.
        // If this fails the processor will be put in the HALT state.
        swapPcPSW (intrptReq.vector ());
}

u8 KD11_NA_ExecutionEngine::cpuPriority ()
{
    return cpuData_->psw ().priorityLevel ();
}

// Fetch PC and PSW from the given vector address. If this fails the
// processor will halt anyway.
bool KD11_NA_ExecutionEngine::fetchFromVector (u16 address, u16* dest)
{
    CondData<u16> tmpValue = mmu_->fetchWord (address);
    *dest = tmpValue.valueOr (0);
    return tmpValue.hasValue ();
}

bool KD11_NA_ExecutionEngine::fetchFromVector (u16 address, function<void (u16)> lambda)
{
    CondData<u16> tmpValue = mmu_->fetchWord (address);
    lambda (tmpValue.valueOr (0));
    return tmpValue.hasValue ();
}

// Swap the PC and PSW with new values from the given vector
void KD11_NA_ExecutionEngine::swapPcPSW (u16 vectorAddress)
{
    trace.cpuEvent (CpuEventRecordType::CPU_TRAP, vectorAddress);

    // Save PC and PSW on the stack. Adressing the stack could result in a
    // bus time out. In that case the CPU is halted.
    if (!mmu_->pushWord (cpuData_->psw ()) || !mmu_->pushWord (cpuData_->registers ()[7]))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, cpuData_->registers ()[6]);
        // ToDo: All interrupts should be cleared?
        cpuData_->clearTrap ();
        runState_ = CpuControl::CpuRunState::HALT;
        haltReason_ = CpuControl::HaltReason::DoubleBusError;
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
        runState_ = CpuControl::CpuRunState::HALT;
        haltReason_ = CpuControl::HaltReason::BusErrorOnIntrptVector;
        bus_->SRUN ().set (false);
        return;
    }
}

void KD11_NA_ExecutionEngine::traceStep ()
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
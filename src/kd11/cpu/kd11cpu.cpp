#include "kd11cpu.h"
#include "qbus/qbus.h"
#include "busdevice/busdevice.h"
#include "float/float.h"
#include "trace/trace.h"
#include "bitmask.h"

#include "kd11_na_instructions/jmp.h"
#include "kd11_na_instructions/rts.h"
#include "kd11_na_instructions/fadd.h"
#include "kd11_na_instructions/fsub.h"
#include "kd11_na_instructions/fmul.h"
#include "kd11_na_instructions/fdiv.h"
#include "kd11_na_instructions/ccc.h"
#include "kd11_na_instructions/scc.h"
#include "kd11_na_instructions/swab.h"
#include "kd11_na_instructions/clr.h"
#include "kd11_na_instructions/com.h"
#include "kd11_na_instructions/inc.h"
#include "kd11_na_instructions/dec.h"
#include "kd11_na_instructions/neg.h"
#include "kd11_na_instructions/adc.h"
#include "kd11_na_instructions/sbc.h"
#include "kd11_na_instructions/tst.h"

#include <functional>
#include <chrono>
#include <memory>

using std::unique_ptr;
using std::make_unique;

// Constructor
KD11CPU::KD11CPU (Qbus* bus)
    :
    bus_ {bus},
    register_ {0},
    psw_ {0},
    runState {CpuRunState::HALT},
    trap_ {nullptr},
    haltReason_ {HaltReason::HaltInstruction}
{
    register_[7] = 0;
    bus_->SRUN().set (false);
}

// Perform a CPU step. The step mainly comprises three actions:
// 1. Execution of an instruction,
// 2. Handle the trace bit,
// 3. Handling of traps and interrupts that might have arisen during execution
//    of the instruction, either a trap as a result of an instruction, or an
//    interrupt requested by a bus device.
// 
// As the power-up mode can be set to trap to the vector at address 024, the
// presence of traps is checked before an instruction is executed.
//
// The normal instruction flow can be interrupted by the setting of the BHALT
// or RESET bus signal. These signals are handled in their corresponding KD11
// receivers which then calls a KD11CPU control function.
//
// This function will return if the CPU is in the state RUN and another
// instruction can be executed, false otherwise. In the latter case a HALT
// instruction was executed.
bool KD11CPU::step ()
{
    // Generate a Trace trap if the trace bit is set, unless the previous
    // instruction was a RTT or another trap is pending.
    if (runState == CpuRunState::INHIBIT_TRACE)
    {
        runState = CpuRunState::RUN;
        bus_->SRUN().set (true);
    }
    else if (!trap_ && (psw_ & PSW_T))
    {
        trace.trap (TrapRecordType::TRAP_T, 014);
        setTrap (&traceTrap);
    }
    handleTraps ();

    // The continutaion of the step depends on the run state:
    // - HALT: the CPU is halted and cannot execute the step, return false,
	// - RUN: the CPU will execute the next instruction,
	// - WAIT: the CPU is running but is still waiting for an interrupt, return true
	// - INHIBIT_TRACE: the CPU is running but no trace trap has to be
    //   executed on this instruction. At this point in the step the cpu cannot
    //   be in this state.
    if (runState == CpuRunState::HALT)
        return false;

    if (runState == CpuRunState::WAIT)
        return true;

    if(trace.isActive ())
    {
        trace.setIgnoreBus ();
        u16 code[3];
        // The read of register_[7]+2 and  register_[7]+4 may access an invalid address as
        // the instruction isn't decoded at this point. Therefore use the bus
        // read function instead of fetchWord(). The latter will generate a
        // bus error trap on access of an invalid address.
        code[0] = bus_->read (register_[7] + 0).valueOr (0);
        code[1] = bus_->read (register_[7] + 2).valueOr (0);
        code[2] = bus_->read (register_[7] + 4).valueOr (0);
        trace.cpuStep (register_, psw_, code);
        trace.clearIgnoreBus ();
    }

    // If there is a pending bus interrupt that can be executed, process
    // that interrupt first, else execute the next instruction
    if (!bus_->intrptReqAvailable () || cpuPriority () >= bus_->intrptPriority ())
    {
        std::chrono::high_resolution_clock::time_point start =
            std::chrono::high_resolution_clock::now ();
        execInstr ();
        std::chrono::high_resolution_clock::time_point end =
            std::chrono::high_resolution_clock::now ();
        trace.duration ("Instruction",
            (std::chrono::duration_cast<std::chrono::nanoseconds> (end - start)).count ());
    }

    return (runState != CpuRunState::HALT);
}

// Execute one instruction
void KD11CPU::execInstr ()
{
    u16 tmp, tmp2;
    u16 src, dst;
    s32 tmps32;

    // Get next instruction to execute and move PC forward
    // u16 insn = fetchWord (register_[7]);
    CondData<u16> tmpValue = fetchWord (register_[7]);
    if (!tmpValue.hasValue())
        return;
    u16 insn = tmpValue;
    register_[7] += 2;

    // single operand instructions
    switch (insn >> 12)
    {
        case 000: 
            // 00 xx xx group
            switch (insn >> 6)
            {
                case 00000: 
                    // 00 00 xx group
                    switch (insn)
                    {
                        case 0000000:
                            HALT (this, insn);
                            break;

                        case 0000001:
                            WAIT (this, insn);
                            break;

                        case 0000002:
                            RTI (this, insn);
                            break;

                        case 0000003:
                            BPT (this, insn);
                            break;

                        case 0000004:
                            IOT (this, insn);
                            break;

                        case 0000005:
                            RESET (this, insn);
                            break;

                        case 0000006:
                            RTT (this, insn);
                            break;

                        default: 
                            // 00 00 07 - 00 00 77
                            unused (this, insn);
                            break;
                    }
                    break;

                case 00001:
                {
                    // JMP (this, insn);
                    unique_ptr<LSI11Instruction> jmp = 
                        make_unique<KD11_NA::JMP> (static_cast<CpuData*> (this), insn);
                    CpuData::Trap returnedTrap = jmp->execute ();
                    if (returnedTrap != CpuData::Trap::None)
                        setTrap (vectorTable [returnedTrap]);
                    break;
                }

                case 00002:
                {
                    // 00 02 xx group
                    if ((insn & 0177770) == 0000200)
                    {
                        // RTS (this, insn);
                        unique_ptr<LSI11Instruction> instr = 
                            make_unique<KD11_NA::RTS> (static_cast<CpuData*> (this), insn);
                        CpuData::Trap returnedTrap = instr->execute ();
                        if (returnedTrap != CpuData::Trap::None)
                            setTrap (vectorTable [returnedTrap]);
                        break;
                    }
                    else if ((insn & 0177740) == 0000240)
                    {
                        unique_ptr<LSI11Instruction> instr;

                        if (insn & 020)
                            // SCC (this, insn);
                            instr = make_unique<KD11_NA::SCC> (static_cast<CpuData*> (this), insn);
                        else
                            // CCC (this, insn);
                            instr = make_unique<KD11_NA::CCC> (static_cast<CpuData*> (this), insn);

                        CpuData::Trap returnedTrap = instr->execute ();
                        if (returnedTrap != CpuData::Trap::None)
                            setTrap (vectorTable [returnedTrap]);
                    }
                    else
                        // 00 02 10 - 00 02 27
                        unused (this, insn);
                    break;
                }

                case 00003:
                {
                    // SWAB (this, insn);
                    unique_ptr<LSI11Instruction> instr = 
                            make_unique<KD11_NA::SWAB> (static_cast<CpuData*> (this), insn);
                    CpuData::Trap returnedTrap = instr->execute ();
                    if (returnedTrap != CpuData::Trap::None)
                            setTrap (vectorTable [returnedTrap]);
                    break;
                }

                case 00004: 
                case 00005:
                case 00006:
                case 00007:
                    BR (this, insn);
                    break;

                case 00010:
                case 00011:
                case 00012:
                case 00013:
                    BNE (this, insn);
                    break;

                case 00014:
                case 00015:
                case 00016:
                case 00017:
                    BEQ (this, insn);
                    break;

                case 00020:
                case 00021:
                case 00022:
                case 00023:
                    BGE (this, insn);
                    break;

                case 00024:
                case 00025:
                case 00026:
                case 00027:
                    BLT (this, insn);
                    break;

                case 00030:
                case 00031:
                case 00032:
                case 00033:
                    BGT (this, insn);
                    break;

                case 00034:
                case 00035:
                case 00036:
                case 00037:
                    BLE (this, insn);
                    break;

                case 00040:
                case 00041:
                case 00042:
                case 00043:
                case 00044:
                case 00045:
                case 00046:
                case 00047:
                    JSR (this, insn);
                    break;

                case 00050:
                {
                    // CLR (this, insn);
                    unique_ptr<LSI11Instruction> instr = 
                        make_unique<KD11_NA::CLR> (static_cast<CpuData*> (this), insn);
                    CpuData::Trap returnedTrap = instr->execute ();
                    if (returnedTrap != CpuData::Trap::None)
                            setTrap (vectorTable [returnedTrap]);
                    break;
                }

                case 00051:
                {
                    // COM (this, insn);
                    unique_ptr<LSI11Instruction> instr = 
                        make_unique<KD11_NA::COM> (static_cast<CpuData*> (this), insn);
                    CpuData::Trap returnedTrap = instr->execute ();
                    if (returnedTrap != CpuData::Trap::None)
                            setTrap (vectorTable [returnedTrap]);
                    break;
                }

                case 00052:
                {
                    // INC (this, insn);
                    unique_ptr<LSI11Instruction> instr = 
                        make_unique<KD11_NA::INC> (static_cast<CpuData*> (this), insn);
                    CpuData::Trap returnedTrap = instr->execute ();
                    if (returnedTrap != CpuData::Trap::None)
                            setTrap (vectorTable [returnedTrap]);
                    break;
                }

                case 00053:
                {
                    // DEC (this, insn);
                    unique_ptr<LSI11Instruction> instr = 
                        make_unique<KD11_NA::DEC> (static_cast<CpuData*> (this), insn);
                    CpuData::Trap returnedTrap = instr->execute ();
                    if (returnedTrap != CpuData::Trap::None)
                            setTrap (vectorTable [returnedTrap]);
                    break;
                }

                case 00054:
                {
                    // NEG (this, insn);
                    unique_ptr<LSI11Instruction> instr = 
                        make_unique<KD11_NA::NEG> (static_cast<CpuData*> (this), insn);
                    CpuData::Trap returnedTrap = instr->execute ();
                    if (returnedTrap != CpuData::Trap::None)
                            setTrap (vectorTable [returnedTrap]);
                    break;
                }

                case 00055:
                {
                    // ADC (this, insn);
                    unique_ptr<LSI11Instruction> instr = 
                        make_unique<KD11_NA::ADC> (static_cast<CpuData*> (this), insn);
                    CpuData::Trap returnedTrap = instr->execute ();
                    if (returnedTrap != CpuData::Trap::None)
                            setTrap (vectorTable [returnedTrap]);
                    break;
                }

                case 00056:
                {
                    // SBC (this, insn);
                    unique_ptr<LSI11Instruction> instr = 
                        make_unique<KD11_NA::SBC> (static_cast<CpuData*> (this), insn);
                    CpuData::Trap returnedTrap = instr->execute ();
                    if (returnedTrap != CpuData::Trap::None)
                            setTrap (vectorTable [returnedTrap]);
                    break;
                }

                case 00057:
                {
                    // TST (this, insn);
                    unique_ptr<LSI11Instruction> instr = 
                        make_unique<KD11_NA::TST> (static_cast<CpuData*> (this), insn);
                    CpuData::Trap returnedTrap = instr->execute ();
                    if (returnedTrap != CpuData::Trap::None)
                            setTrap (vectorTable [returnedTrap]);
                    break;
                }

                case 00060:
                    ROR (this, insn);
                    break;

                case 00061:
                    ROL (this, insn);
                    break;

                case 00062:
                    ASR (this, insn);
                    break;

                case 00063:
                    ASL (this, insn);
                    break;

                case 00064:
                    MARK (this, insn);
                    break;

                case 00067:
                    SXT (this, insn);
                    break;

                default:
                    // 006500-006677, 007000-007777
                    unused (this, insn);
                    break;
            }
            break;

        case 001:
            MOV (this, insn);
            break;

        case 002:
            CMP (this, insn);
            break;

        case 003:
            BIT (this, insn);
            break;

        case 004:
            BIC (this, insn);
            break;

        case 005:
            BIS (this, insn);
            break;

        case 006:
            ADD (this, insn);
            break;

        case 007: 
            // 07 xx xx group 
            switch (insn >> 9)
            {
                case 0070:
                    MUL (this, insn);
                    break;

                case 0071:
                    DIV (this, insn);
                    break;

                case 0072:
                    ASH (this, insn);
                    break;

                case 0073:
                    ASHC (this, insn);
                    break;

                case 0074:
                    XOR (this, insn);
                    break;

                case 0075: /* FIS instructions */
                    switch (insn >> 3)
                    {
#ifdef USE_FLOAT
                        case 007500:
                        {
                            // FADD (this, insn);
                            unique_ptr<LSI11Instruction> instr = 
                                make_unique<KD11_NA::FADD> (static_cast<CpuData*> (this), insn);
                            CpuData::Trap returnedTrap = instr->execute ();
                            if (returnedTrap != CpuData::Trap::None)
                                setTrap (vectorTable [returnedTrap]);
                            break;
                        }

                        case 007501:
                        {
                            // FSUB (this, insn);
                            unique_ptr<LSI11Instruction> instr = 
                                make_unique<KD11_NA::FSUB> (static_cast<CpuData*> (this), insn);
                            CpuData::Trap returnedTrap = instr->execute ();
                            if (returnedTrap != CpuData::Trap::None)
                                setTrap (vectorTable [returnedTrap]);
                            break;
                        }

                        case 007502:
                        {
                            // FMUL (this, insn);
                            unique_ptr<LSI11Instruction> instr = 
                                make_unique<KD11_NA::FMUL> (static_cast<CpuData*> (this), insn);
                            CpuData::Trap returnedTrap = instr->execute ();
                            if (returnedTrap != CpuData::Trap::None)
                                setTrap (vectorTable [returnedTrap]);
                            break;
                        }

                        case 007503:
                        {
                            // FDIV (this, insn);
                            unique_ptr<LSI11Instruction> instr = 
                                make_unique<KD11_NA::FDIV> (static_cast<CpuData*> (this), insn);
                            CpuData::Trap returnedTrap = instr->execute ();
                            if (returnedTrap != CpuData::Trap::None)
                                setTrap (vectorTable [returnedTrap]);
                            break;
                        }
#endif
                        default:
                            // 075040-076777: unused 
                            unused (this, insn);
                            break;
                    }
                    break;

                case 0077:
                    SOB (this, insn);
                    break;

                default:
                    unused (this, insn);
                    break;
            }
            break;

        case 010: 
            // 10 xx xx group 
            switch (insn >> 6)
            {
                case 01000:
                case 01001:
                case 01002:
                case 01003:
                    BPL (this, insn);
                    break;

                case 01004:
                case 01005:
                case 01006:
                case 01007:
                    BMI (this, insn);
                    break;

                case 01010:
                case 01011:
                case 01012:
                case 01013:
                    BHI (this, insn);
                    break;

                case 01014:
                case 01015:
                case 01016:
                case 01017:
                    BLOS (this, insn);
                    break;

                case 01020:
                case 01021:
                case 01022:
                case 01023:
                    BVC (this, insn);
                    break;

                case 01024:
                case 01025:
                case 01026:
                case 01027:
                    BVS (this, insn);
                    break;

                case 01030:
                case 01031:
                case 01032:
                case 01033:
                    BCC (this, insn);
                    break;

                case 01034:
                case 01035:
                case 01036:
                case 01037:
                    BCS (this, insn);
                    break;

                case 01040:
                case 01041:
                case 01042:
                case 01043:
                    EMT (this, insn);
                    break;

                case 01044:
                case 01045:
                case 01046:
                case 01047:
                    TRAP (this, insn);
                    break;

                case 01050:
                    CLRB (this, insn);
                    break;

                case 01051:
                    COMB (this, insn);
                    break;

                case 01052:
                    INCB (this, insn);
                    break;

                case 01053:
                    DECB (this, insn);
                    break;

                case 01054:
                    NEGB (this, insn);
                    break;

                case 01055:
                    ADCB (this, insn);
                    break;

                case 01056:
                    SBCB (this, insn);
                    break;

                case 01057:
                    TSTB (this, insn);
                    break;

                case 01060:
                    RORB (this, insn);
                    break;

                case 01061:
                    ROLB (this, insn);
                    break;

                case 01062:
                    ASRB (this, insn);
                    break;

                case 01063:
                    ASLB (this, insn);
                    break;

                case 01064:
                    MTPS (this, insn);
                    break;

                case 01067:
                    MFPS (this, insn);
                    break;

                default:
                    unused (this, insn);
                    break;
            }
            break;

        case 011:
            MOVB (this, insn);
            break;

        case 012:
            CMPB (this, insn);
            break;

        case 013:
            BITB (this, insn);
            break;

        case 014:
            BICB (this, insn);
            break;

        case 015:
            BISB (this, insn);
            break;

        case 016:
            SUB (this, insn);
            break;

        default:
            unused (this, insn);
            break;
    }
}


// This function is called on every KD11 step, whether or not a trap to
// be handled is present. If a trap is present the current PC and PSW are
// saved on the stack and the PC and PSW of the trap vector are loaded.
// If there is no trap to be handled the function simply returns.
//
// Trap priority order from high to low (cf Fig. 2-13) is defined as (vectors
// between brackets):
// - Bus error (004)
// - Machine trap (BPT (014), IOT (020), EMT (030), TRAP (034) instruction)
// - Trace trap (PSW bit 4) (014)
// - Powerfail/HALT interrupt (024)
// - Event interrupt (LTC) (100)
// - Device interrupt
// 
// The event and device interrupts are only processed if the PSW priority bit
// is cleared.
// 
// Refer to the LSI-11 WCS user's guide (EK-KUV11-TM-001) par 2.3.
//

void KD11CPU::handleTraps ()
{
    InterruptRequest intrptReq;
    u16 trapToProcess{ 0 };
    CondData<u16> tmpValue;

    // Traps are handled in order of their priority:
    // - Bus errors,
    // - Instruction traps
    // - Event and device interrupts, only if the priority bit is clear,
    // 
    // Interrupts are only processed if their priority is higher than the
    // current CPU priority. (The LSI-11 has just two priority levels,
    // zero and BR4.) Note that the numerical value of the TrapPriority enum
    // is used as bus request level. Traps in HALT mode are ignored.
    // 
    // Check if there is a trap or interrupt request to handle and the CPU
    // isn't halted. This is the most common case so check this as first.
    if ((!trap_ && !bus_->intrptReqAvailable ()) || runState == CpuRunState::HALT)
        return;

    // Traps have the highest priority, so first check if there is a trap
    // to handle
    if (trap_)
    {
        trapToProcess = trap_->vector ();
        trap_ = nullptr;
    }
    else if (bus_->intrptPriority () > cpuPriority ())
    {
        if (bus_->getIntrptReq (intrptReq))
            trapToProcess = intrptReq.vector ();
        else
            return;
    }
    else return;


    trace.cpuEvent (CpuEventRecordType::CPU_TRAP, trapToProcess);

    // Save PC and PSW on the stack. Adressing the stack could result in a
    // bus time out. In that case the CPU is halted.
    // ToDo: Remove code duplication
    register_[6] -= 2;
    if (!putWord (register_[6], psw_))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, register_[6]);
        // ToDo: All interrupts should be cleared?
        trap_ = nullptr;
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::DoubleBusError;
        bus_->SRUN().set (false);
        return;
    }

    register_[6] -= 2;
    if (!putWord (register_[6], register_[7]))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, register_[6]);
        trap_ = nullptr;
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::DoubleBusError;
        bus_->SRUN().set (false);
        return;
    }

    // Read new PC and PSW from the trap vector. These read's could also
    // result in a bus time out.
    tmpValue = fetchWord (trapToProcess);
    register_[7] = tmpValue.valueOr (0);
    if (!tmpValue.hasValue ())
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, trapToProcess);
        trap_ = nullptr;
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::BusErrorOnIntrptVector;
        bus_->SRUN().set (false);
        return;
    }

    tmpValue = fetchWord (trapToProcess + 2);
    psw_ = tmpValue.valueOr (0);
    if (!tmpValue.hasValue ())
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, trapToProcess + 2);
        trap_ = nullptr;
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::BusErrorOnIntrptVector;
        bus_->SRUN().set (false);
        return;
    }

    /* resume execution if in WAIT state */
    if (runState == CpuRunState::WAIT)
    {
        trace.cpuEvent (CpuEventRecordType::CPU_RUN, register_[7]);
        runState = CpuRunState::RUN;
        bus_->SRUN().set (true);
    }
}

// Load PC and PSW with the vector from the given trap
void KD11CPU::loadTrapVector (InterruptRequest const* trap)
{
    unsigned char trapVector = trap->vector ();
    register_[7] = fetchWord (trapVector).valueOr (0);
    psw_ = fetchWord (trapVector + 2).valueOr (0);
}

// Generate the given trap using the interrupt request mechanism
void KD11CPU::setTrap (InterruptRequest const* trap)
{
    trap_ = trap;
}

CpuRunState KD11CPU::currentCpuState ()
{
    return runState;
}

u8 KD11CPU::cpuPriority()
{
    return (psw_ & PSW_PRIORITY) >> 5;
}
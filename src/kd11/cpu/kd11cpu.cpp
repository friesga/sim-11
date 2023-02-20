#include "kd11cpu.h"

#include "instructions.h"
#include "qbus/qbus.h"
#include "busdevice/busdevice.h"
#include "float/float.h"
#include "trace/trace.h"
#include "bitmask.h"

#include <functional>
#include <chrono>

#define RETURN_IF(cond) \
    if (cond) return;

// Constructor
KD11CPU::KD11CPU (Qbus* bus)
    :
    runState {STATE_HALT},
    r {0},
    bus_ {bus},
    psw {0},
    trap_ {nullptr}
{
    r[7] = bootAddress;
    bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::False);
}

CondData<u16> KD11CPU::fetchWord (u16 address)
{
    // return bus_->read (address);
    CondData<u16> value = bus_->read (address);
    if (!value.hasValue ())
    {
        trace.bus (BusRecordType::ReadFail, address, 0);
        setTrap (&busError);
        return {};
    }
    return value;
}

bool KD11CPU::putWord (u16 address, u16 value)
{
    if (!bus_->writeWord (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        setTrap (&busError);
        return false;
    }
    return true;
}

bool KD11CPU::putByte (u16 address, u8 value)
{
    if (!bus_->writeByte (address, value))
    {
        trace.bus (BusRecordType::WriteFail, address, value);
        setTrap (&busError);
        return false;
    }
    return true;
}

// Return the result of a floating point calculation
void KD11CPU::returnFISresult (Float result, u16 registerNumber)
{
    u16 high, low;
    Float::Result conversionResult = result.pdp11Dword (&high, &low);
    if (conversionResult == Float::Result::OK)
    {
        putWord (r[registerNumber] + 4, high);
        putWord (r[registerNumber] + 6, low);
        r[registerNumber] += 4;
        PSW_EQ (PSW_N, result.value () < 0);
        PSW_EQ (PSW_Z, result.value () == 0);
        PSW_CLR (PSW_V);
        PSW_CLR (PSW_C);
    }
    else if (conversionResult == Float::Result::Underflow)
    {
        PSW_SET (PSW_N);
        PSW_SET (PSW_V);
        PSW_CLR (PSW_Z);
        PSW_CLR (PSW_C);
        setTrap (&FIS);
    }
    else
    {
        // Overflow or Nan
        PSW_CLR (PSW_N);
        PSW_SET (PSW_V);
        PSW_CLR (PSW_Z);
        PSW_CLR (PSW_C);
        setTrap (&FIS);
    }
}

#define	READCPU(addr)	\
    (tmpValue = fetchWord(addr)).valueOr(0);

// Perform a CPU step. The step mainly comprises four actions:
// 1. Check the state of the BHALT and BDCOK bus signals. If one of these
//    signals is true, the normal instruction flow is interrupted and the
//    corresponding action is performed,
// 2. Execution of an instruction,
// 3. Handle the trace bit,
// 4. Handling of traps and interrupts that might have arisen during execution
//    of the instruction, either a trap as a result of an instruction, or an
//    interrupt requested by a bus device.
void KD11CPU::step ()
{
    if (bus_->signalIsSet (Qbus::Signal::BHALT))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_HALT, r[7]);
        runState = STATE_HALT;
        bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::False);
        return;
    }

    if (bus_->signalIsSet (Qbus::Signal::BDCOK))
    {
        r[7] = bootAddress;
        psw = 0;
        bus_->clearInterrupts ();
        trace.cpuEvent (CpuEventRecordType::CPU_ODT_G, bootAddress);
    }

    if(trace.isActive ())
    {
        trace.setIgnoreBus ();
        u16 code[3];
        // The read of r[7]+2 and  r[7]+4 may access an invalid address as
        // the instruction isn't decoded at this point. Therefore use the bus
        // read function instead of fetchWord(). The latter will generate a
        // bus error trap on access of an invalid address.
        code[0] = bus_->read (r[7] + 0).valueOr (0);
        code[1] = bus_->read (r[7] + 2).valueOr (0);
        code[2] = bus_->read (r[7] + 4).valueOr (0);
        trace.cpuStep (r, psw, code);
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

    // Generate a Trace trap if the trace bit is set, unless the previous
    // instruction was a RTT or another trap is pending.
    if (runState == STATE_INHIBIT_TRACE)
    {
        runState = STATE_RUN;
        bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::True);
    }
    else if (!trap_ && (psw & PSW_T))
    {
        trace.trap (TrapRecordType::TRAP_T, 014);
        setTrap (&traceTrap);
    }
    handleTraps ();
}

// Execute one instruction
void KD11CPU::execInstr ()
{
    u16 tmp, tmp2;
    u16 src, dst;
    s32 tmps32;

    // Get next instruction to execute and move PC forward
    // u16 insn = fetchWord (r[7]);
    CondData<u16> tmpValue = fetchWord (r[7]);
    if (!tmpValue.hasValue())
        return;
    u16 insn = tmpValue;
    r[7] += 2;

    // Get pointers to the possible instruction formats
    KD11INSN1* insn1 = (KD11INSN1*)&insn;
    KD11INSN2* insn2 = (KD11INSN2*)&insn;
    KD11INSNBR* insnbr = (KD11INSNBR*)&insn;
    KD11INSNJSR* insnjsr = (KD11INSNJSR*)&insn;
    KD11INSNRTS* insnrts = (KD11INSNRTS*)&insn;
    KD11INSNMARK* insnmark = (KD11INSNMARK*)&insn;
    KD11INSNSOB* insnsob = (KD11INSNSOB*)&insn;

    /* single operand instructions */
    switch (insn >> 12)
    {
        case 000: /* 00 xx xx group */
            switch (insn >> 6)
            {
                case 00000: /* 00 00 xx group */
                    switch (insn)
                    {
                        case 0000000: /* HALT */
                            trace.cpuEvent (CpuEventRecordType::CPU_HALT, r[7]);

                            runState = STATE_HALT;
                            bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::False);
                            // The ODT state is set to ODT_STATE_INIT in 
                            // KD11:step() when it detects the runState HALT.
                            // odt.state = ODT_STATE_INIT;
                            break;

                        case 0000001: /* WAIT */
                            trace.cpuEvent (CpuEventRecordType::CPU_WAIT, r[7]);
                            runState = STATE_WAIT;
                            break;

                        case 0000002: /* RTI */
                            r[7] = READCPU (r[6]);
                            r[6] += 2;
                            RETURN_IF (!tmpValue.hasValue ());
                            psw = READCPU (r[6]);
                            r[6] += 2;
                            RETURN_IF (!tmpValue.hasValue ());
                            break;

                        case 0000003: /* BPT */
                            trace.trap (TrapRecordType::TRAP, 014);
                            setTrap (&BPT);
                            break;

                        case 0000004: /* IOT */
                            trace.trap (TrapRecordType::TRAP, 020);
                            setTrap (&IOT);
                            break;

                        case 0000005: /* RESET */
                            bus_->reset ();
                            break;

                        case 0000006: /* RTT */
                            r[7] = READCPU (r[6]);
                            r[6] += 2;
                            RETURN_IF (!tmpValue.hasValue ());
                            psw = READCPU (r[6]);
                            r[6] += 2;
                            RETURN_IF (!tmpValue.hasValue ());

                            // Prevent a trace trap on the next instruction
                            runState = STATE_INHIBIT_TRACE;
                            break;

                        default: /* 00 00 07 - 00 00 77 */
                            /* unused opcodes */
                            trace.trap (TrapRecordType::TRAP_ILL, 010);
                            setTrap (&illegalInstructionTrap);
                            break;
                    }
                    break;

                case 00001: /* JMP */
                    if (!insn1->getAddress (this, r, r[7]))
                    {
                        // Illegal instruction
                        trace.trap (TrapRecordType::TRAP_RADDR, 04);
                        setTrap (&busError);
                    }
                    break;

                case 00002: /* 00 02 xx group */
                    /* mask=177740: CLN/CLZ/CLV/CLC/CCC/SEN/SEZ/SEV/SEC/SCC */
                    if ((insn & 0177770) == 0000200)
                    {
                        /* RTS */
                        r[7] = r[insnrts->rn];
                        r[insnrts->rn] = fetchWord (r[6]);
                        r[6] += 2;
                    }
                    else if ((insn & 0177740) == 0000240)
                    {
                        tmp = insn & 017;
                        if (insn & 020)
                        {
                            psw |= tmp;
                        }
                        else
                        {
                            psw &= ~tmp;
                        }
                    }
                    else
                    {
                        /* 00 02 10 - 00 02 27: unused */
                        trace.trap (TrapRecordType::TRAP_ILL, 010);
                        setTrap (&illegalInstructionTrap);
                    }
                    break;

                case 00003: /* SWAB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;
                    tmp = ((tmp & 0x00FF) << 8) | ((tmp >> 8) & 0xFF);

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !((u8)tmp));
                    PSW_CLR (PSW_V);
                    PSW_CLR (PSW_C);
                    break;

                case 00004: /* BR */
                case 00005:
                case 00006:
                case 00007:
                    r[7] += (s16)((s8)insnbr->offset) * 2;
                    break;
                case 00010: /* BNE */
                case 00011:
                case 00012:
                case 00013:
                    if (!PSW_GET (PSW_Z))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 00014: /* BEQ */
                case 00015:
                case 00016:
                case 00017:
                    if (PSW_GET (PSW_Z))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 00020: /* BGE */
                case 00021:
                case 00022:
                case 00023:
                    if ((PSW_GET (PSW_N) ^ PSW_GET (PSW_V)) == 0)
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 00024: /* BLT */
                case 00025:
                case 00026:
                case 00027:
                    if (PSW_GET (PSW_N) ^ PSW_GET (PSW_V))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 00030: /* BGT */
                case 00031:
                case 00032:
                case 00033:
                    if ((PSW_GET (PSW_Z) || (PSW_GET (PSW_N) ^ PSW_GET (PSW_V))) == 0)
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 00034: /* BLE */
                case 00035:
                case 00036:
                case 00037:
                    if (PSW_GET (PSW_Z) || (PSW_GET (PSW_N) ^ PSW_GET (PSW_V)))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 00040: /* JSR */
                case 00041:
                case 00042:
                case 00043:
                case 00044:
                case 00045:
                case 00046:
                case 00047:
                {
                    bool ok = insn1->getAddress (this, r, dst);
                    src = r[insnjsr->r];
                    if (!ok)
                    {
                        // Illegal instruction
                        trace.trap (TrapRecordType::TRAP_RADDR, 04);
                        setTrap (&busError);
                        return;
                    }
                }
                r[6] -= 2;
                putWord (r[6], src);
                r[insnjsr->r] = r[7];
                r[7] = dst;
                break;

                case 00050: /* CLR */
                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), 0))
                        return;
                    PSW_CLR (PSW_N | PSW_V | PSW_C);
                    PSW_SET (PSW_Z);
                    break;

                case 00051: /* COM */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;
                    tmp = ~tmp;

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;
                    PSW_CLR (PSW_V);
                    PSW_SET (PSW_C);
                    PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 00052: /* INC */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word), src))
                        return;
                    tmp = src + 1;

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    PSW_EQ (PSW_V, src == 077777)
                        PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 00053: /* DEC */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word), src))
                        return;
                    tmp = src - 1;

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    PSW_EQ (PSW_V, src == 0100000)
                        PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 00054: /* NEG */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    if (tmp != 0100000)
                        tmp = -tmp;

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    PSW_EQ (PSW_V, tmp == 0100000)
                        PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    PSW_EQ (PSW_C, tmp);
                    break;

                case 00055: /* ADC */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word), src))
                        return;

                    tmp2 = PSW_GET (PSW_C) ? 1 : 0;
                    tmp = src + tmp2;

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    PSW_EQ (PSW_V, src == 0077777 && PSW_GET (PSW_C));
                    PSW_EQ (PSW_C, src == 0177777 && PSW_GET (PSW_C));
                    PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 00056: /* SBC */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word), src))
                        return;

                    tmp2 = PSW_GET (PSW_C) ? 1 : 0;
                    tmp = src - tmp2;

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    PSW_EQ (PSW_V, src == 0100000);
                    PSW_EQ (PSW_C, !src && PSW_GET (PSW_C));
                    PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 00057: /* TST */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word |
                            OperandOptions::AutoIncr), tmp))
                        return;

                    PSW_CLR (PSW_V);
                    PSW_CLR (PSW_C);
                    PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 00060: /* ROR */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word), src))
                        return;
                    tmp2 = PSW_GET (PSW_C);
                    tmp = src >> 1;
                    if (tmp2)
                        tmp |= 0x8000;

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    PSW_EQ (PSW_C, src & 0x0001);
                    PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
                    break;

                case 00061: /* ROL */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word), src))
                        return;
                    tmp2 = PSW_GET (PSW_C);
                    tmp = src << 1;
                    if (tmp2)
                        tmp |= 0x0001;

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    PSW_EQ (PSW_C, src & 0x8000);
                    PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
                    break;

                case 00062: /* ASR */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word), src))
                        return;
                    tmp = src;
                    if (tmp & 0x8000)
                    {
                        tmp >>= 1;
                        tmp |= 0x8000;
                    }
                    else
                        tmp >>= 1;

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    PSW_EQ (PSW_C, src & 1);
                    PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
                    break;

                case 00063: /* ASL */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Word), src))
                        return;
                    tmp = src << 1;

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    PSW_EQ (PSW_C, src & 0x8000);
                    PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
                    break;

                case 00064: /* MARK */
                    r[6] = r[7] + 2 * insnmark->nn;
                    r[7] = r[5];
                    r[5] = fetchWord (r[6]);
                    r[6] += 2;
                    break;

                case 00067: /* SXT */
                    if (PSW_GET (PSW_N))
                    {
                        tmp = 0xFFFF;
                    }
                    else
                    {
                        tmp = 0;
                    }

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Word), tmp))
                        return;

                    PSW_EQ (PSW_Z, !PSW_GET (PSW_N));
                    PSW_CLR (PSW_V);
                    break;

                default: /* 006500-006677, 007000-007777: unused */
                    trace.trap (TrapRecordType::TRAP_ILL, 010);
                    setTrap (&illegalInstructionTrap);
                    break;
            }
            break;

        case 001: /* MOV */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Word |
                    OperandOptions::AutoIncr), tmp))
                return;

            if (!insn2->putDestOperand (this, r,
                Bitmask (OperandOptions::Word), tmp))
                return;

            PSW_EQ (PSW_N, tmp & 0x8000);
            PSW_EQ (PSW_Z, !tmp);
            PSW_CLR (PSW_V);
            break;

        case 002: /* CMP */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Word |
                    OperandOptions::AutoIncr), src))
                return;

            if (!insn2->getDestOperand (this, r,
                Bitmask (OperandOptions::Word |
                    OperandOptions::AutoIncr), dst))
                return;

            tmp = src - dst;
            PSW_EQ (PSW_N, tmp & 0x8000);
            PSW_EQ (PSW_Z, !tmp);
            PSW_EQ (PSW_V, ((src & 0x8000) != (dst & 0x8000)) \
                && ((dst & 0x8000) == (tmp & 0x8000)));
            PSW_EQ (PSW_C, ((u32)src - (u32)dst) & 0x10000);
            break;

        case 003: /* BIT */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Word |
                    OperandOptions::AutoIncr), src))
                return;

            if (!insn2->getDestOperand (this, r,
                Bitmask (OperandOptions::Word |
                    OperandOptions::AutoIncr), dst))
                return;

            tmp = src & dst;
            PSW_EQ (PSW_N, tmp & 0x8000);
            PSW_EQ (PSW_Z, !tmp);
            PSW_CLR (PSW_V);
            break;

        case 004: /* BIC */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Word |
                    OperandOptions::AutoIncr), src))
                return;

            if (!insn2->getDestOperand (this, r,
                Bitmask (OperandOptions::Word), dst))
                return;

            tmp = ~src & dst;

            if (!insn2->putDestOperand (this, r,
                Bitmask (OperandOptions::Word), tmp))
                return;

            PSW_EQ (PSW_N, tmp & 0x8000);
            PSW_EQ (PSW_Z, !tmp);
            PSW_CLR (PSW_V);
            break;

        case 005: /* BIS */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Word |
                    OperandOptions::AutoIncr), src))
                return;

            if (!insn2->getDestOperand (this, r,
                Bitmask (OperandOptions::Word), dst))
                return;

            tmp = src | dst;

            if (!insn2->putDestOperand (this, r,
                Bitmask (OperandOptions::Word), tmp))
                return;

            PSW_EQ (PSW_N, tmp & 0x8000);
            PSW_EQ (PSW_Z, !tmp);
            PSW_CLR (PSW_V);
            break;

        case 006: /* ADD */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Word |
                    OperandOptions::AutoIncr), src))
                return;

            if (!insn2->getDestOperand (this, r,
                Bitmask (OperandOptions::Word), dst))
                return;

            tmp = src + dst;

            if (!insn2->putDestOperand (this, r,
                Bitmask (OperandOptions::Word), tmp))
                return;

            PSW_EQ (PSW_N, tmp & 0x8000);
            PSW_EQ (PSW_Z, !tmp);
            PSW_EQ (PSW_V, ((src & 0x8000) == (dst & 0x8000)) \
                && ((dst & 0x8000) != (tmp & 0x8000)));
            PSW_EQ (PSW_C, ((u32)src + (u32)dst) & 0x10000);
            break;

        case 007: /* 07 xx xx group */
            switch (insn >> 9)
            {
                case 0070: /* MUL */
                    dst = r[insnjsr->r];
                    if (!insnjsr->getOperand (this, r,
                        Bitmask (OperandOptions::Word |
                            OperandOptions::AutoIncr), src))
                        return;

                    tmps32 = (s32)(s16)dst * (s16)src;
                    r[insnjsr->r] = (u16)(tmps32 >> 16);
                    r[insnjsr->r | 1] = (u16)tmps32;
                    PSW_CLR (PSW_V);
                    PSW_EQ (PSW_N, tmps32 < 0);
                    PSW_EQ (PSW_Z, !tmps32);
                    PSW_EQ (PSW_C, (tmps32 >= 0x7FFF) || (tmps32 < -0x8000));
                    break;

                case 0071: /* DIV */
                    tmps32 = (r[insnjsr->r] << 16) | r[insnjsr->r | 1];

                    if (!insnjsr->getOperand (this, r,
                        Bitmask (OperandOptions::Word |
                            OperandOptions::AutoIncr), src))
                        return;

                    if (src == 0)
                    {
                        PSW_SET (PSW_C);
                        PSW_SET (PSW_V);
                    }
                    else
                    {
                        s32 quot = tmps32 / (s16)src;
                        s32 rem = tmps32 % (s16)src;
                        PSW_CLR (PSW_C);
                        if ((s16)quot != quot)
                        {
                            PSW_SET (PSW_V);
                        }
                        else
                        {
                            r[insnjsr->r] = (u16)quot;
                            r[insnjsr->r | 1] = (u16)rem;
                            PSW_EQ (PSW_Z, !quot);
                            PSW_EQ (PSW_N, quot < 0);
                        }
                    }
                    break;
                case 0072: /* ASH */
                    dst = r[insnjsr->r];

                    if (!insnjsr->getOperand (this, r,
                        Bitmask (OperandOptions::Word |
                            OperandOptions::AutoIncr), src))
                        return;

                    if (src & 0x20)
                    { /* negative; right */
                        s16 stmp, stmp2;
                        src = (~src & 0x1F) + 1;
                        stmp = (s16)dst;
                        stmp2 = stmp >> (src - 1);
                        stmp >>= src;
                        tmp = (u16)stmp;
                        PSW_EQ (PSW_C, stmp2 & 1);
                        PSW_CLR (PSW_V);
                    }
                    else if ((src & 0x1F) == 0)
                    {
                        /* nothing */
                        PSW_CLR (PSW_V);
                        PSW_CLR (PSW_C);
                        tmp = dst;
                    }
                    else
                    { /* positive, left */
                        s16 mask = 0;
                        src &= 0x1F;
                        tmp = dst << src;
                        if (src > 0)
                        {
                            mask = 0x8000;
                            mask >>= src;
                            tmp2 = dst & mask;
                            PSW_EQ (PSW_V, !((tmp2 == 0) || (((tmp2 & mask) | ~mask) == 0xFFFF)));
                        }
                        else
                        {
                            PSW_CLR (PSW_V);
                        }
                        PSW_EQ (PSW_C, (dst << (src - 1)) & 0x8000);
                        if ((dst & 0x8000) != (tmp & 0x8000))
                        {
                            PSW_SET (PSW_V);
                        }
                    }
                    r[insnjsr->r] = tmp;
                    PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 0073: /* ASHC */
                    dst = r[insnjsr->r];
                    tmps32 = (r[insnjsr->r] << 16) | r[insnjsr->r | 1];

                    if (!insnjsr->getOperand (this, r,
                        Bitmask (OperandOptions::Word |
                            OperandOptions::AutoIncr), src))
                        return;

                    if ((src & 0x3F) == 0x20)
                    { /* negative; 32 right */
                        PSW_EQ (PSW_C, tmps32 & 0x80000000);
                        PSW_CLR (PSW_V);
                        if (PSW_GET (PSW_C))
                        {
                            tmps32 = 0xFFFFFFFF;
                        }
                        else
                        {
                            tmps32 = 0;
                        }
                    }
                    else if (src & 0x20)
                    { /* negative; right */
                        s32 stmp2;
                        src = (~src & 0x1F) + 1;
                        stmp2 = tmps32 >> (src - 1);
                        tmps32 >>= src;
                        PSW_EQ (PSW_C, stmp2 & 1);
                    }
                    else if ((src & 0x1F) == 0)
                    {
                        /* nothing */
                        PSW_CLR (PSW_V);
                        PSW_CLR (PSW_C);
                    }
                    else
                    { /* positive, left */
                        s32 stmp2;
                        src &= 0x1F;
                        stmp2 = tmps32 << (src - 1);
                        tmps32 <<= src;
                        PSW_EQ (PSW_C, stmp2 & 0x80000000);
                        PSW_EQ (PSW_V, !!(dst & 0x8000)
                            != !!(tmps32 & 0x80000000));
                    }
                    r[insnjsr->r] = (u16)(tmps32 >> 16);
                    r[insnjsr->r | 1] = (u16)tmps32;
                    PSW_EQ (PSW_N, tmps32 & 0x80000000);
                    PSW_EQ (PSW_Z, !tmps32);
                    break;

                case 0074: /* XOR */
                    src = r[insnjsr->r];

                    if (!insnjsr->getOperand (this, r,
                        Bitmask (OperandOptions::Word), dst))
                        return;

                    tmp = src ^ dst;

                    if (!insnjsr->putOperand (this, r, tmp))
                        return;

                    PSW_EQ (PSW_N, tmp & 0x8000);
                    PSW_EQ (PSW_Z, !tmp);
                    PSW_CLR (PSW_V);
                    break;

                case 0075: /* FIS instructions */
                    switch (insn >> 3)
                    {
#ifdef USE_FLOAT
                        case 007500: /* FADD */
                            executeFISinstruction (insnrts->rn,
                                [] (Float f1, Float f2) {return true;},
                                [] (Float f1, Float f2) {return f1 + f2;});
                            break;

                        case 007501: /* FSUB */
                            executeFISinstruction (insnrts->rn,
                                [] (Float f1, Float f2) {return true;},
                                [] (Float f1, Float f2) {return f1 - f2;});
                            break;

                        case 007502: /* FMUL */
                            executeFISinstruction (insnrts->rn,
                                [] (Float f1, Float f2) {return true;},
                                [] (Float f1, Float f2) {return f1 * f2;});
                            break;

                        case 007503: /* FDIV */
                            executeFISinstruction (insnrts->rn,
                                [] (Float f1, Float f2) {return f2.value() != 0;},
                                [] (Float f1, Float f2) {return f1 / f2;});
                            break;
#endif
                        default:
                            /* 075040-076777: unused */
                            trace.trap (TrapRecordType::TRAP_ILL, 010);
                            setTrap (&illegalInstructionTrap);
                            break;
                    }
                    break;
                case 0077: /* SOB */
                    r[insnsob->rn]--;
                    if (r[insnsob->rn])
                    {
                        r[7] -= 2 * insnsob->offset;
                    }
                    break;
                default:
                    trace.trap (TrapRecordType::TRAP_ILL, 010);
                    setTrap (&illegalInstructionTrap);
                    break;
            }
            break;

        case 010: /* 10 xx xx group */
            switch (insn >> 6)
            {
                case 01000: /* BPL */
                case 01001:
                case 01002:
                case 01003:
                    if (!PSW_GET (PSW_N))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;

                case 01004: /* BMI */
                case 01005:
                case 01006:
                case 01007:
                    if (PSW_GET (PSW_N))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;

                case 01010: /* BHI */
                case 01011:
                case 01012:
                case 01013:
                    if (!PSW_GET (PSW_C) && !PSW_GET (PSW_Z))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;

                case 01014: /* BLOS */
                case 01015:
                case 01016:
                case 01017:
                    if (PSW_GET (PSW_C) || PSW_GET (PSW_Z))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;

                case 01020: /* BVC */
                case 01021:
                case 01022:
                case 01023:
                    if (!PSW_GET (PSW_V))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;

                case 01024: /* BVS */
                case 01025:
                case 01026:
                case 01027:
                    if (PSW_GET (PSW_V))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;

                case 01030: /* BCC */
                case 01031:
                case 01032:
                case 01033:
                    if (!PSW_GET (PSW_C))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;

                case 01034: /* BCS */
                case 01035:
                case 01036:
                case 01037:
                    if (PSW_GET (PSW_C))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;

                case 01040: /* EMT */
                case 01041:
                case 01042:
                case 01043:
                    trace.trap (TrapRecordType::TRAP, 030);
                    setTrap (&EMT);
                    break;

                case 01044: /* TRAP */
                case 01045:
                case 01046:
                case 01047:
                    trace.trap (TrapRecordType::TRAP, 034);
                    setTrap (&TRP);
                    break;

                case 01050: /* CLRB */
                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Byte), 0))
                        return;

                    PSW_CLR (PSW_N | PSW_V | PSW_C);
                    PSW_SET (PSW_Z);
                    break;

                case 01051: /* COMB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    tmp = ~tmp;

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    PSW_CLR (PSW_V);
                    PSW_SET (PSW_C);
                    PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !((u8)tmp));
                    break;

                case 01052: /* INCB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte), src))
                        return;

                    tmp = (u8)(src + 1);

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    PSW_EQ (PSW_V, src == 000177)
                        PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 01053: /* DECB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte), src))
                        return;

                    tmp = (u8)(src - 1);

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    PSW_EQ (PSW_V, src == 0000200)
                        PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 01054: /* NEGB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    if (tmp != 0200)
                    {
                        tmp = -tmp;
                    }
                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    PSW_EQ (PSW_V, tmp == 0200)
                        PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !tmp);
                    PSW_EQ (PSW_C, tmp);
                    break;

                case 01055: /* ADCB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte), src))
                        return;

                    tmp2 = PSW_GET (PSW_C) ? 1 : 0;
                    tmp = (u8)(src + tmp2);

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    PSW_EQ (PSW_V, src == 0177 && PSW_GET (PSW_C));
                    PSW_EQ (PSW_C, src == 0377 && PSW_GET (PSW_C));
                    PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 01056: /* SBCB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte), src))
                        return;

                    tmp2 = PSW_GET (PSW_C) ? 1 : 0;
                    tmp = (u8)(src - tmp2);

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    PSW_EQ (PSW_V, src == 0200);
                    PSW_EQ (PSW_C, !src && PSW_GET (PSW_C));
                    PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 01057: /* TSTB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte |
                            OperandOptions::AutoIncr), tmp))
                        return;

                    PSW_CLR (PSW_V);
                    PSW_CLR (PSW_C);
                    PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !tmp);
                    break;

                case 01060: /* RORB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte), src))
                        return;

                    tmp2 = PSW_GET (PSW_C);
                    tmp = src >> 1;
                    if (tmp2)
                    {
                        tmp |= 0x80;
                    }

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    PSW_EQ (PSW_C, src & 0x01);
                    PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !tmp);
                    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
                    break;

                case 01061: /* ROLB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte), src))
                        return;

                    tmp2 = PSW_GET (PSW_C);
                    tmp = (u8)(src << 1);
                    if (tmp2)
                    {
                        tmp |= 0x01;
                    }

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    PSW_EQ (PSW_C, src & 0x80);
                    PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !tmp);
                    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
                    break;

                case 01062: /* ASRB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte), src))
                        return;

                    tmp = src;
                    if (tmp & 0x80)
                    {
                        tmp >>= 1;
                        tmp |= 0x80;
                    }
                    else
                    {
                        tmp >>= 1;
                    }

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    PSW_EQ (PSW_C, src & 1);
                    PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !tmp);
                    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
                    break;

                case 01063: /* ASLB */
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte), src))
                        return;

                    tmp = (u8)(src << 1);

                    if (!insn1->putOperand (this, r,
                        Bitmask (OperandOptions::Byte), tmp))
                        return;

                    PSW_EQ (PSW_C, src & 0x80);
                    PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !tmp);
                    PSW_EQ (PSW_V, PSW_GET (PSW_N) ^ PSW_GET (PSW_C));
                    break;

                case 01064:
                    // MTPS
                    // Note that the T bit cannot be set with this instruction
                    // and that bits 5 and 6 are reserved.
                    if (!insn1->getOperand (this, r,
                        Bitmask (OperandOptions::Byte |
                            OperandOptions::AutoIncr), tmp))
                        return;


                    // Allow bits 5/6/7 to be set and cleared
                    psw = (psw & PSW_T) | (tmp & ~PSW_T);
                    break;

                case 01067: /* MFPS */
                    tmp = (u8)psw;
                    if (insn1->mode == 0)
                    {
                        // If destination is mode 0, PS bit 7 is sign
                        // extended through upper byte of the register.
                        r[insn1->rn] = (s8) psw;
                    }
                    else
                    {
                        if (!insn1->putOperand (this, r,
                            Bitmask (OperandOptions::Byte), tmp))
                            return;
                    }
                    PSW_EQ (PSW_N, tmp & 0x80);
                    PSW_EQ (PSW_Z, !(tmp & 0xFF));
                    PSW_CLR (PSW_V);
                    break;

                default:
                    /* unused */
                    trace.trap (TrapRecordType::TRAP_ILL, 010);
                    setTrap (&illegalInstructionTrap);
                    break;
            }
            break;

        case 011: /* MOVB */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Byte |
                    OperandOptions::AutoIncr), tmp))
                return;
            tmp = (s8)tmp;
            if (insn2->dst_mode == 0)
            {
                r[insn2->dst_rn] = tmp;
            }
            else
            {
                if (!insn2->putDestOperand (this, r,
                    Bitmask (OperandOptions::Byte), tmp))
                    return;
            }
            PSW_EQ (PSW_N, tmp & 0x80);
            PSW_EQ (PSW_Z, !tmp);
            PSW_CLR (PSW_V);
            break;

        case 012: /* CMPB */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Byte |
                    OperandOptions::AutoIncr), src))
                return;

            if (!insn2->getDestOperand (this, r,
                Bitmask (OperandOptions::Byte |
                    OperandOptions::AutoIncr), dst))
                return;

            tmp = (u8)(src - dst);
            PSW_EQ (PSW_N, tmp & 0x80);
            PSW_EQ (PSW_Z, !tmp);
            PSW_EQ (PSW_V, ((src & 0x80) != (dst & 0x80)) \
                && ((dst & 0x80) == (tmp & 0x80)));
            PSW_EQ (PSW_C, (src - dst) & 0x100);
            break;

        case 013: /* BITB */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Byte |
                    OperandOptions::AutoIncr), src))
                return;

            if (!insn2->getDestOperand (this, r,
                Bitmask (OperandOptions::Byte |
                    OperandOptions::AutoIncr), dst))
                return;

            tmp = src & dst;
            PSW_EQ (PSW_N, tmp & 0x80);
            PSW_EQ (PSW_Z, !tmp);
            PSW_CLR (PSW_V);
            break;

        case 014: /* BICB */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Byte |
                    OperandOptions::AutoIncr), src))
                return;

            if (!insn2->getDestOperand (this, r,
                Bitmask (OperandOptions::Byte), dst))
                return;

            tmp = (u8)(~src & dst);

            if (!insn2->putDestOperand (this, r,
                Bitmask (OperandOptions::Byte), tmp))
                return;

            PSW_EQ (PSW_N, tmp & 0x80);
            PSW_EQ (PSW_Z, !tmp);
            PSW_CLR (PSW_V);
            break;

        case 015: /* BISB */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Byte |
                    OperandOptions::AutoIncr), src))
                return;

            if (!insn2->getDestOperand (this, r,
                Bitmask (OperandOptions::Byte), dst))
                return;

            tmp = src | dst;

            if (!insn2->putDestOperand (this, r,
                Bitmask (OperandOptions::Byte), tmp))
                return;

            PSW_EQ (PSW_N, tmp & 0x80);
            PSW_EQ (PSW_Z, !tmp);
            PSW_CLR (PSW_V);
            break;

        case 016: /* SUB */
            if (!insn2->getSourceOperand (this, r,
                Bitmask (OperandOptions::Word |
                    OperandOptions::AutoIncr), src))
                return;

            if (!insn2->getDestOperand (this, r,
                Bitmask (OperandOptions::Word), dst))
                return;

            tmp = dst - src;

            if (!insn2->putDestOperand (this, r,
                Bitmask (OperandOptions::Word), tmp))
                return;

            PSW_EQ (PSW_N, tmp & 0x8000);
            PSW_EQ (PSW_Z, !tmp);
            PSW_EQ (PSW_V, ((src & 0x8000) != (dst & 0x8000)) \
                && ((src & 0x8000) == (tmp & 0x8000)));
            PSW_EQ (PSW_C, ((u32)dst - (u32)src) & 0x10000);
            break;

        default: /* unused */
            trace.trap (TrapRecordType::TRAP_ILL, 010);
            setTrap (&illegalInstructionTrap);
            break;
    }
}

// Execute a FADD, FSUB, FMUL or FDIV instruction.
void KD11CPU::executeFISinstruction (u16 stackPointer,
    std::function<bool(Float, Float)> argumentsValid,
    std::function<Float(Float, Float)> instruction)
{
    // Clear PSW bits 5 and 6
    psw &= ~(_BV(5) | _BV(6));

    CondData<u16> f1High = fetchWord (r[stackPointer] + 4);
    CondData<u16> f1Low = fetchWord (r[stackPointer] + 6);
    CondData<u16> f2High = fetchWord (r[stackPointer]);
    CondData<u16> f2Low = fetchWord (r[stackPointer] + 2);

    if (f1High.hasValue () && f1Low.hasValue () &&
        f2High.hasValue () && f2Low.hasValue ())
    {
        Float f1 (f1High, f1Low);
        Float f2 (f2High, f2Low);

        if (argumentsValid (f1, f2))
        {
            Float f3 = instruction (f1, f2);
            returnFISresult (f3, stackPointer);
        }
        else
        {
            // The arguments are invalid. This is notably a division
            // by zero
            PSW_SET (PSW_N);
            PSW_CLR (PSW_Z);
            PSW_SET (PSW_V);
            PSW_SET (PSW_C);
            setTrap (&FIS);
        }
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
    if ((!trap_ && !bus_->intrptReqAvailable ()) || runState == STATE_HALT)
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
    r[6] -= 2;
    if (!putWord (r[6], psw))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, r[6]);
        // ToDo: All interrupts should be cleared?
        trap_ = nullptr;
        runState = STATE_HALT;
        bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::False);
        return;
    }

    r[6] -= 2;
    if (!putWord (r[6], r[7]))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, r[6]);
        trap_ = nullptr;
        runState = STATE_HALT;
        bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::False);
        return;
    }

    // Read new PC and PSW from the trap vector. These read's could also
    // result in a bus time out.
    tmpValue = fetchWord (trapToProcess);
    r[7] = tmpValue.valueOr (0);
    if (!tmpValue.hasValue ())
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, trapToProcess);
        trap_ = nullptr;
        runState = STATE_HALT;
        bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::False);
        return;
    }

    tmpValue = fetchWord (trapToProcess + 2);
    psw = tmpValue.valueOr (0);
    if (!tmpValue.hasValue ())
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, trapToProcess + 2);
        trap_ = nullptr;
        runState = STATE_HALT;
        bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::False);
        return;
    }

    /* resume execution if in WAIT state */
    if (runState == STATE_WAIT)
    {
        trace.cpuEvent (CpuEventRecordType::CPU_RUN, r[7]);
        runState = STATE_RUN;
        bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::True);
    }
}

// Generate the given trap using the interrupt request mechanism
void KD11CPU::setTrap (InterruptRequest const* trap)
{
    trap_ = trap;
}

u8 KD11CPU::cpuPriority()
{
    return (psw & PSW_PRIORITY) >> 5;
}
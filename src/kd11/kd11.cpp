#include "trace/trace.h"
#include "kd11.h"
#include "instructions.h"

#include <string.h>
#include <stdlib.h>


#define	TRAP(trap)		setTrap(bus, &trap)

// Constructor
// ToDo: Reinitialize K11ODT
KD11CPU::KD11CPU()
	:
	runState{0},
	r{0}, 
	psw{0},
    trap_ {nullptr}
{}

void KD11::reset()
{
	cpu_.reset();
	odt.reset();
}

KD11CPU &KD11::cpu()
{
	return cpu_;
}

#define	READ(addr)			(bus->read((addr)))
#define	WRITE(addr, val)	(bus->writeWord((addr), (val)))

#define RETURN_IF(cond) \
    if (cond) return;

// Constructor
KD11ODT::KD11ODT(KD11CPU &cpu)
	:
	cpu_{cpu},
	state{ODT_STATE_INIT}
{}

void KD11ODT::reset()
{
	state = ODT_STATE_INIT;
}

void KD11ODT::clear()
{
	buf_sz = 0;
}

void KD11ODT::write(u8 c)
{
	buf[buf_sz++] = c;
	buf_r = 0;
}

void KD11ODT::writeOctal(u16 val)
{
	int i;
	buf[buf_sz++] = ((val >> 15) & 0x7) + '0';
	for(i = 0; i < 5; i++)
	{
		buf[buf_sz++] = ((val >> 12) & 7) + '0';
		val <<= 3;
	}
}

void KD11ODT::inputError()
{
	state = ODT_STATE_WR;
	next = ODT_STATE_WAIT;
	buf[0] = '?';
	buf[1] = '\r';
	buf[2] = '\n';
	buf[3] = '@';
	buf_r = 0;
	buf_sz = 4;
}

void KD11ODT::step(QBUS* bus)
{
	/* odt */
	// KD11ODT* odt = &odt;
	switch(state)
	{
		case ODT_STATE_INIT:
			clear();
			write('\r');
			write('\n');
			writeOctal(cpu_.r[7]);
			write('\r');
			write('\n');
			write('@');
			next = ODT_STATE_WAIT;
			state = ODT_STATE_WR;
			break;

		case ODT_STATE_WAIT:
			if (READ(0177560) & 0x80) 
			{
				/* ch available */
				u16 c = READ(0177562);
				WRITE(0177566, (u8) c);
				switch((u8) c) 
				{
					case '$':
					case 'R':
						state = ODT_STATE_REG;
						break;
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
						state = ODT_STATE_ADDR;
						addr = ((u8) c) - '0';
						break;

					case 'G':
						clear();
						write('\r');
						write('\n');
						write('@');
						next = ODT_STATE_WAIT;
						state = ODT_STATE_WR;
						break;

					case 'P':
						state = ODT_STATE_INIT;
						cpu_.runState = STATE_RUN;
						TRCCPUEvent(TRC_CPU_ODT_P, cpu_.r[7]);
						break;

					default:
						inputError();
						break;
				}
			}
			break;

		case ODT_STATE_WR:
			if (READ(0177564) & 0x80)
			{
				WRITE(0177566, buf[buf_r++]);
				if (buf_r == buf_sz)
					state = next;
			}
			break;

		case ODT_STATE_ADDR:
			if (READ(0177560) & 0x80) 
			{ 
				/* ch available */
				u16 ch = READ(0177562);
				u8 c = (u8) ch;
				WRITE(0177566, c);
				if ((u8) c == '/') 
				{ 
					/* delimit */
                    // ToDo: The address might be invalid and in that case
                    // ODT should respond with a '?' instead of a zero.
					u16 val = READ(addr).valueOr(0);
					clear();
					writeOctal(val);
					write(' ');
					next = ODT_STATE_VAL;
					state = ODT_STATE_WR;
					input = 0;
				} 
				else if (c >= '0' && c <= '7') 
				{
					addr <<= 3;
					addr |= c - '0';
				} 
				else if(c == 'G') 
				{
					state = ODT_STATE_INIT;
					cpu_.r[7] = addr;
					cpu_.runState = STATE_RUN;
					TRCCPUEvent(TRC_CPU_ODT_G, addr);
				} 
				else 
					inputError();
			}
			break;

		case ODT_STATE_REG:
			if (READ(0177560) & 0x80) 
			{ 
				/* ch available */
				u16 ch = READ(0177562);
				u8 c = (u8) ch;
				WRITE(0177566, c);
				state = ODT_STATE_REG_WAIT;
				if (c >= '0' && c <= '7') 
					addr = c - '0'; 
				else if (c == 'S') 
					addr = 8;
				else
					inputError();
			}
			break;

		case ODT_STATE_VAL:
			if (READ(0177560) & 0x80)
			{ 
				/* ch available */
				u16 ch = READ(0177562);
				u8 c = (u8) ch;
				WRITE(0177566, c);
				if (c == '\r' || c == '\n') 
				{
					if (input)
						WRITE(addr, val);
				} 
				else if(c >= '0' && c <= '7') 
				{
					val <<= 3;
					val |= c - '0';
					input = 1;
				} 
				else 
					inputError();

				if(c == '\r')
				{
					clear();
					write('\r');
					write('\n');
					write('@');
					state = ODT_STATE_WR;
					next = ODT_STATE_WAIT;
				} 
				else if (c == '\n') 
				{
					u16 tmpValue;

					addr += 2;
					val = 0;
					tmpValue = READ(addr);

					clear();
					write('\r');
					write('\n');
					writeOctal(addr);
					write('/');
					writeOctal(tmpValue);
					write(' ');

					next = ODT_STATE_VAL;
					state = ODT_STATE_WR;
					input = 0;
				}
			}
			break;

		case ODT_STATE_REG_WAIT:
			if (READ(0177560) & 0x80) 
			{ 
				/* ch available */
				u16 ch = READ(0177562);
				u8 c = (u8) ch;
				WRITE(0177566, c);
				if (c == '/') 
				{
					u16 val;
					if (addr < 8) 
						val = cpu_.r[addr];
					else 
						val = cpu_.psw;

					clear();
					writeOctal(val);
					write(' ');
					val = 0;
					next = ODT_STATE_REG_VAL;
					state = ODT_STATE_WR;
					input = 0;
				} 
				else 
					inputError();
			}
			break;

		case ODT_STATE_REG_VAL:
			if (READ(0177560) & 0x80) 
			{ 
				/* ch available */
				u16 ch = READ(0177562);
				u8 c = (u8) ch;
				WRITE(0177566, c);
				if (c == '\r' || c == '\n') 
				{
					if (input) 
					{
						if (addr == 8) 
							cpu_.psw = val;
						else 
							cpu_.r[addr] = val;
					}
				} 
				else if (c >= '0' && c <= '7') 
				{
					val <<= 3;
					val |= c - '0';
					input = 1;
				} else 
					inputError();

				if (c == '\r' || (c == '\n' && addr == 7)) 
				{
					clear();
					write('\r');
					write('\n');
					write('@');
					state = ODT_STATE_WR;
					next = ODT_STATE_WAIT;
				} 
				else if (c == '\n') 
				{
					u16 tmpValue;

					addr++;
					val = 0;
					tmpValue = cpu_.r[addr];

					clear();
					write('\r');
					write('R');
					write(addr + '0');
					write('/');
					writeOctal(tmpValue);
					write(' ');

					next = ODT_STATE_REG_VAL;
					state = ODT_STATE_WR;
					input = 0;
				}
			}
			break;
	}
}

void KD11CPU::reset()
{
	r[7] = 0173000;
	psw = 0;
	trap_ = nullptr;
	runState = STATE_HALT;
}

CondData<u16> KD11CPU::readW(QBUS* bus, u16 dst, u16 mode, int inc)
{
	CondData<u16> addr;
	switch(mode)
	{
		case 0: /* Register */
			return r[dst];

		case 1: /* Register indirect */
			return READ(r[dst]);

		case 2: /* Autoincrement */
			addr = r[dst];
			if(inc) {
				r[dst] += 2;
				r[dst] &= 0xFFFE;
			}
			return READ(addr);

		case 3: /* Autoincrement indirect */
			addr = r[dst];
			if(inc) {
				r[dst] += 2;
				r[dst] &= 0xFFFE;
			}
			addr = READ(addr);
            if (!addr.hasValue()) return{};
			return READ(addr);

		case 4: /* Autodecrement */
			if(inc) {
				r[dst] -= 2;
				addr = r[dst];
				r[dst] &= 0xFFFE;
			} else {
				addr = r[dst] - 2;
			}
			return READ(addr);

		case 5: /* Autodecrement indirect */
			if(inc) {
				r[dst] -= 2;
				addr = r[dst];
				r[dst] &= 0xFFFE;
			} else {
				addr = r[dst] - 2;
			}
			addr = READ(addr);
            if (!addr.hasValue()) return{};
			return READ(addr);

		case 6: /* Index */
			addr = READ(r[7]);
            if (!addr.hasValue()) return{};
			if(inc) {
				r[7] += 2;
			} else if(dst == 7) {
				addr += 2;
			}
			addr += r[dst];
			return READ(addr);

		case 7: /* Index indirect */
			addr = READ(r[7]);
            if (!addr.hasValue()) return{};
			if(inc) {
				r[7] += 2;
			} else if(dst == 7) {
				addr += 2;
			}
			addr += r[dst];
			addr = READ(addr);
            if (!addr.hasValue()) return{};
			return READ(addr);

		default:
			// return 0;
			return {};
	}
}

#define	READ8(addr) (((addr) & 1) ? \
    static_cast<CondData<u8>> (READ((addr) & 0xFFFE) >> 8) : \
	static_cast<CondData<u8>> (READ(addr & 0xFFFE)))

CondData<u8> KD11CPU::readB(QBUS* bus, u16 dst, u16 mode, int inc)
{
	CondData<u16> addr;
	switch(mode)
	{
		case 0: /* Register */
			return (u8) r[dst];

		case 1: /* Register indirect */
			return READ8(r[dst]);

		case 2: /* Autoincrement */
			addr = r[dst];
			if(inc) {
				if(dst == 6 || dst == 7) {
					r[dst] += 2;
				} else {
					r[dst]++;
				}
			}
			return READ8(addr);

		case 3: /* Autoincrement indirect */
			addr = r[dst];
			if(inc) {
				r[dst] += 2;
			}
			addr = READ(addr);
            if (!addr.hasValue()) return{};
			return READ8(addr);

		case 4: /* Autodecrement */
			if(inc) {
				if(dst == 6 || dst == 7) {
					r[dst] -= 2;
				} else {
					r[dst]--;
				}
				addr = r[dst];
			} else {
				if(dst == 6 || dst == 7) {
					addr = r[dst] - 2;
				} else {
					addr = r[dst] - 1;
				}
			}
			return READ8(addr);

		case 5: /* Autodecrement indirect */
			if(inc) {
				r[dst] -= 2;
				addr = r[dst];
			} else {
				addr = r[dst] - 2;
			}
			addr = READ(addr);
            if (!addr.hasValue()) return{};
			return READ8(addr);

		case 6: /* Index */
			addr = READ(r[7]);
            if (!addr.hasValue()) return{};
			if(inc) {
				r[7] += 2;
			} else if(dst == 7) {
				addr += 2;
			}
			addr += r[dst];
			return READ8(addr);

		case 7: /* Index indirect */
			addr = READ(r[7]);
            if (!addr.hasValue()) return{};
			if(inc) {
				r[7] += 2;
			} else if(dst == 7) {
				addr += 2;
			}
			addr += r[dst];
			addr = READ(addr);
            if (!addr.hasValue()) return{};
			return READ8(addr);

		default:
			// return 0;
			return {};
	}
}

bool KD11CPU::writeW(QBUS* bus, u16 dst, u16 mode, u16 val)
{
	CondData<u16> addr;
	switch(mode)
    {
		case 0: /* Register */
			r[dst] = val;
            return true;

		case 1: /* Register indirect */
			return WRITE(r[dst], val);

		case 2: /* Autoincrement */
			r[dst] &= 0xFFFE;
			addr = r[dst];
			r[dst] += 2;
			return WRITE(addr, val);

		case 3: /* Autoincrement indirect */
			r[dst] &= 0xFFFE;
			addr = r[dst];
			r[dst] += 2;
			addr = READ(addr);
            if (!addr.hasValue())
                return false;
			return WRITE(addr, val);

		case 4: /* Autodecrement */
			r[dst] &= 0xFFFE;
			r[dst] -= 2;
			addr = r[dst];
			return WRITE(addr, val);

		case 5: /* Autodecrement indirect */
			r[dst] &= 0xFFFE;
			r[dst] -= 2;
			addr = r[dst];
			addr = READ(addr);
            if (!addr.hasValue())
                return false;
			return WRITE(addr, val);

		case 6: /* Index */
			addr = READ(r[7]);
            if (!addr.hasValue())
                return false;
			r[7] += 2;
			addr += r[dst];
			return WRITE(addr, val);

		case 7: /* Index indirect */
			addr = READ(r[7]);
            if (!addr.hasValue())
                return false;
			r[7] += 2;
			addr += r[dst];
			addr = READ(addr);
            if (!addr.hasValue())
                return false;
			return WRITE(addr, val);

        default:
            // Prevent compiler warning on not all paths returning a value
            throw (std::string("Unexpected mode in writeW() call"));
	}
}


bool KD11CPU::writeB(QBUS* bus, u16 dst, u16 mode, u8 val)
{
	CondData<u16> addr;
	switch(mode)
    {
		case 0: /* Register */
			r[dst] = (r[dst] & 0xFF00) | val;
			return true;

		case 1: /* Register deferred */
			return bus->writeByte (r[dst], val);

		case 2: /* Autoincrement */
			addr = r[dst];
			if(dst == 6 || dst == 7) {
				r[dst] += 2;
			} else {
				r[dst]++;
			}
			return bus->writeByte (addr, val);

		case 3: /* Autoincrement deferred */
			addr = r[dst];
			r[dst] += 2;
			addr = READ(addr);
            if (!addr.hasValue())
                return false;
			return bus->writeByte (addr, val);

		case 4: /* Autodecrement */
			if(dst == 6 || dst == 7) {
				r[dst] -= 2;
			} else {
				r[dst]--;
			}
			addr = r[dst];
			return bus->writeByte (addr, val);

		case 5: /* Autodecrement deferred */
			r[dst] -= 2;
			addr = r[dst];
			addr = READ(addr);
            if (!addr.hasValue())
                return false;
			return bus->writeByte (addr, val);

		case 6: /* Index */
			addr = READ(r[7]);
            if (!addr.hasValue())
                return false;
			r[7] += 2;
			addr += r[dst];
			return bus->writeByte (addr, val);

		case 7: /* Index deferred */
			addr = READ(r[7]);
            if (!addr.hasValue())
                return false;
			r[7] += 2;
			addr += r[dst];
			addr = READ(addr);
            if (!addr.hasValue())
                return false;
			return bus->writeByte (addr, val);

        default:
            // Prevent compiler warning on not all paths returning a value
            throw (std::string("Unexpected mode in writeB() call"));
	}
}

// ToDo: The result of all read() calls should be checked (instead of
// returning zero).
CondData<u16> KD11CPU::getAddr(QBUS* bus, u16 dst, u16 mode)
{
	CondData<u16> addr;
	switch(mode)
    {
		case 0: /* Register */
			TRCTrap(4, TRC_TRAP_RADDR);
			TRAP(busError); /* illegal instruction */
            return {};

		case 1: /* Register indirect */
			return r[dst];

		case 2: /* Autoincrement */
			addr = r[dst];
			r[dst] += 2;
			return addr;

		case 3: /* Autoincrement indirect */
			addr = r[dst];
			r[dst] += 2;
			addr = READ(addr);
			return addr;

		case 4: /* Autodecrement */
			r[dst] -= 2;
			addr = r[dst];
			return addr;

		case 5: /* Autodecrement indirect */
			r[dst] -= 2;
			addr = r[dst];
			addr = READ(addr);
			return addr;

		case 6: /* Index */
			addr = READ(r[7]);
			r[7] += 2;
			addr += r[dst];
			return addr;

		case 7: /* Index indirect */
			addr = READ(r[7]);
			r[7] += 2;
			addr += r[dst];
			addr = READ(addr);
			return addr;

		default:
            return {};
	}
}

// The following macro's read a word or byte from the bus in the
// form of a CondData object. If the object contains a valid value
// that value is returned by the macro, otherwise the value 0 is
// returned. Subsequently the CondData object is used to check if
// a valid value was returned and in that case the macro call executes
// a return.

#define	READCPU(addr)	\
    (tmpValue = bus->read(addr)).valueOr(0);

#define	CPUREADW(rn, mode)	\
    (tmpValue = readW(bus, rn, mode, 1)).valueOr(0); \
	RETURN_IF(!tmpValue.hasValue())
#define	CPUREADB(rn, mode)	\
    (tmpValue = static_cast<CondData<u16>> (readB(bus, rn, mode, 1))).valueOr(0); \
	RETURN_IF(!tmpValue.hasValue())
#define	CPUREADNW(rn, mode)	\
    (tmpValue = readW(bus, rn, mode, 0)).valueOr(0);

#define	CPUREADNB(rn, mode)	\
    (tmpValue = static_cast<CondData<u16>> (readB(bus, rn, mode, 0))).valueOr(0); \
	RETURN_IF(!tmpValue.hasValue())

#define CPUWRITEW(rn, mode, v)	\
    if (!writeW(bus, rn, mode, v)) \
	    return;
#define CPUWRITEB(rn, mode, v)	\
    if (!writeB(bus, rn, mode, (u8) v)) \
	    return;

typedef union {
	float	_f32;
	u32		_u32;
} FLOAT;

// Perform a CPU step. The step mainly comprises three actions:
// 1. Execution of an instruction,
// 2. Handle the trace bit,
// 3. Handling of traps and interrupts that might have arisen during execution
//    of the instruction, either a trap as a result of an instruction, or an
//    interrupt requested by a bus device.
void KD11CPU::step(QBUS* bus)
{
    IFTRC()
    {
        TRCSETIGNBUS();
        u16 code[3];
        code[0] = READ(r[7] + 0);
        code[1] = READ(r[7] + 2);
        code[2] = READ(r[7] + 4);
        TRCStep(r, psw, code);
        TRCCLRIGNBUS();
    }
    execInstr(bus);

    // Generate a Trace trap if the trace bit is set, unless the previous
    // instruction was a RTT or another trap is pending.
    if (runState == STATE_INHIBIT_TRACE)
        runState = STATE_RUN;
    else if (!trap_ && (psw & PSW_T))
    {
        TRCTrap(014, TRC_TRAP_T);
        TRAP(traceTrap);
    }
    handleTraps(bus);
}

// Execute one instruction
void KD11CPU::execInstr(QBUS* bus)
{
    u16 tmp, tmp2;
    u16 src, dst;
    s32 tmps32;
    CondData<u16> tmpValue;     // Used in CPUREAD* macro's
#ifdef USE_FLOAT
    FLOAT f1, f2, f3;
#endif

    // If there is a pending bus interrupt that can be executed, process
    // that interrupt first
    if (bus->intrptReqAvailable() && bus->intrptPriority() > cpuPriority())
        return;

    // Get next instruction to execute and move PC forward
    u16 insn = READ(r[7]);
    r[7] += 2;

    // Get pointers to the possible instruction formats
    KD11INSN1* insn1 = (KD11INSN1*) &insn;
    KD11INSN2* insn2 = (KD11INSN2*) &insn;
    KD11INSNBR* insnbr = (KD11INSNBR*) &insn;
    KD11INSNJSR* insnjsr = (KD11INSNJSR*) &insn;
    KD11INSNRTS* insnrts = (KD11INSNRTS*) &insn;
    KD11INSNMARK* insnmark = (KD11INSNMARK*) &insn;
    KD11INSNSOB* insnsob = (KD11INSNSOB*) &insn;



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
                            TRCCPUEvent(TRC_CPU_HALT, r[7]);

                            runState = STATE_HALT;
                            // The ODT state is set to ODT_STATE_INIT in 
                            // KD11:step() when it detects the runState HALT.
                            // odt.state = ODT_STATE_INIT;
                            break;

                        case 0000001: /* WAIT */
                            TRCCPUEvent(TRC_CPU_WAIT, r[7]);
                            runState = STATE_WAIT;
                            break;

                        case 0000002: /* RTI */
                            r[7] = READCPU(r[6]);
                            r[6] += 2;
                            RETURN_IF(!tmpValue.hasValue());
                            psw = READCPU (r[6]);
                            r[6] += 2;
                            RETURN_IF(!tmpValue.hasValue());
                            break;

                        case 0000003: /* BPT */
                            TRCTrap(014, TRC_TRAP);
                            TRAP(BPT);
                            break;

                        case 0000004: /* IOT */
                            TRCTrap(020, TRC_TRAP);
                            TRAP(IOT);
                            break;

                        case 0000005: /* RESET */
                            bus->reset();
                            break;

                        case 0000006: /* RTT */
                            r[7] = READCPU(r[6]);
                            r[6] += 2;
                            RETURN_IF(!tmpValue.hasValue());
                            psw = READCPU(r[6]);
                            r[6] += 2;
                            RETURN_IF(!tmpValue.hasValue());

                            // Prevent a trace trap on the next instruction
                            runState = STATE_INHIBIT_TRACE;
                            break;

                        default: /* 00 00 07 - 00 00 77 */
                            /* unused opcodes */
                            TRCTrap(010, TRC_TRAP_ILL);
                            TRAP(illegalInstructionTrap);
                            break;
                    }
                    break;

                case 00001: /* JMP */
                    tmpValue = getAddr(bus, insn1->rn, insn1->mode);
                    RETURN_IF(!tmpValue.hasValue());
                    r[7] = tmpValue.value();
                    break;

                case 00002: /* 00 02 xx group */
                    /* mask=177740: CLN/CLZ/CLV/CLC/CCC/SEN/SEZ/SEV/SEC/SCC */
                    if ((insn & 0177770) == 0000200)
                    {
                        /* RTS */
                        r[7] = r[insnrts->rn];
                        r[insnrts->rn] = READ(r[6]);
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
                        TRCTrap(010, TRC_TRAP_ILL);
                        TRAP(illegalInstructionTrap);
                    }
                    break;
                case 00003: /* SWAB */
                    // tmp = CPUREADNW(insn1->rn, insn1->mode);
                    // RETURN_IF(!tmpValue.hasValue());
                    if (!insn1->getOperand(bus, r, Bitmask(GetOperandOptions::Word), tmp))
                        return;
                    tmp = ((tmp & 0x00FF) << 8) | ((tmp >> 8) & 0xFF);
                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !((u8)tmp));
                    PSW_CLR(PSW_V);
                    PSW_CLR(PSW_C);
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
                    if (!PSW_GET(PSW_Z))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 00014: /* BEQ */
                case 00015:
                case 00016:
                case 00017:
                    if (PSW_GET(PSW_Z))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 00020: /* BGE */
                case 00021:
                case 00022:
                case 00023:
                    if ((PSW_GET(PSW_N) ^ PSW_GET(PSW_V)) == 0)
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 00024: /* BLT */
                case 00025:
                case 00026:
                case 00027:
                    if (PSW_GET(PSW_N) ^ PSW_GET(PSW_V))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 00030: /* BGT */
                case 00031:
                case 00032:
                case 00033:
                    if ((PSW_GET(PSW_Z) || (PSW_GET(PSW_N) ^ PSW_GET(PSW_V))) == 0)
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 00034: /* BLE */
                case 00035:
                case 00036:
                case 00037:
                    if (PSW_GET(PSW_Z) || (PSW_GET(PSW_N) ^ PSW_GET(PSW_V)))
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
                    tmpValue = getAddr(bus, insnjsr->rn, insnjsr->mode);
                    src = r[insnjsr->r];
                    RETURN_IF(!tmpValue.hasValue());
                    dst = tmpValue.value();

                    r[6] -= 2;
                    WRITE(r[6], src);
                    r[insnjsr->r] = r[7];
                    r[7] = dst;
                    break;

                case 00050: /* CLR */
                    CPUWRITEW(insn1->rn, insn1->mode, 0);
                    PSW_CLR(PSW_N | PSW_V | PSW_C);
                    PSW_SET(PSW_Z);
                    break;
                case 00051: /* COM */
                    // tmp = CPUREADNW(insn1->rn, insn1->mode);
                    // RETURN_IF(!tmpValue.hasValue());
                    if (!insn1->getOperand(bus, r, Bitmask(GetOperandOptions::Word), tmp))
                        return;
                    tmp = ~tmp;
                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_CLR(PSW_V);
                    PSW_SET(PSW_C);
                    PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    break;
                case 00052: /* INC */
                    // src = CPUREADNW(insn1->rn, insn1->mode);
                    // src = (tmpValue = insn1->getWordOperand(bus, r, false).valueOr(0));
                    // RETURN_IF(!tmpValue.hasValue());
                    if (!insn1->getOperand(bus, r, Bitmask(GetOperandOptions::Word), src))
                        return;
                    tmp = src + 1;
                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_V, src == 077777)
                        PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    break;
                case 00053: /* DEC */
                    // src = CPUREADNW(insn1->rn, insn1->mode);
                    // src = (tmpValue = insn1->getWordOperand(bus, r, false).valueOr(0));
                    // RETURN_IF(!tmpValue.hasValue());
                    if (!insn1->getOperand(bus, r, Bitmask(GetOperandOptions::Word), src))
                        return;
                    tmp = src - 1;
                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_V, src == 0100000)
                        PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    break;
                case 00054: /* NEG */
                    // tmp = CPUREADNW(insn1->rn, insn1->mode);
                    // RETURN_IF(!tmpValue.hasValue());
                    if (!insn1->getOperand(bus, r, Bitmask(GetOperandOptions::Word), tmp))
                        return;

                    if (tmp != 0100000)
                        tmp = -tmp;

                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_V, tmp == 0100000)
                        PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    PSW_EQ(PSW_C, tmp);
                    break;

                case 00055: /* ADC */
                    // src = CPUREADNW(insn1->rn, insn1->mode);
                    // RETURN_IF(!tmpValue.hasValue());
                    if (!insn1->getOperand(bus, r, Bitmask(GetOperandOptions::Word), src))
                        return;

                    tmp2 = PSW_GET(PSW_C) ? 1 : 0;
                    tmp = src + tmp2;
                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_V, src == 0077777 && PSW_GET(PSW_C));
                    PSW_EQ(PSW_C, src == 0177777 && PSW_GET(PSW_C));
                    PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    break;

                case 00056: /* SBC */
                    // src = CPUREADNW(insn1->rn, insn1->mode);
                    // RETURN_IF(!tmpValue.hasValue());
                    if (!insn1->getOperand(bus, r, Bitmask(GetOperandOptions::Word), src))
                        return;

                    tmp2 = PSW_GET(PSW_C) ? 1 : 0;
                    tmp = src - tmp2;
                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_V, src == 0100000);
                    PSW_EQ(PSW_C, !src && PSW_GET(PSW_C));
                    PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    break;

                case 00057: /* TST */
                    // tmp = CPUREADW(insn1->rn, insn1->mode);
                    if (!insn1->getOperand(bus, r, 
                        Bitmask(GetOperandOptions::Word |
                            GetOperandOptions::AutoIncr), tmp))
                        return;

                    PSW_CLR(PSW_V);
                    PSW_CLR(PSW_C);
                    PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    break;

                case 00060: /* ROR */
                    // src = CPUREADNW(insn1->rn, insn1->mode);
                    // RETURN_IF(!tmpValue.hasValue());
                    if (!insn1->getOperand(bus, r, Bitmask(GetOperandOptions::Word), src))
                        return;
                    tmp2 = PSW_GET(PSW_C);
                    tmp = src >> 1;
                    if (tmp2)
                        tmp |= 0x8000;

                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_C, src & 0x0001);
                    PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
                    break;

                case 00061: /* ROL */
                    // src = CPUREADNW(insn1->rn, insn1->mode);
                    // RETURN_IF(!tmpValue.hasValue());
                    if (!insn1->getOperand(bus, r, Bitmask(GetOperandOptions::Word), src))
                        return;
                    tmp2 = PSW_GET(PSW_C);
                    tmp = src << 1;
                    if (tmp2)
                        tmp |= 0x0001;

                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_C, src & 0x8000);
                    PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
                    break;

                case 00062: /* ASR */
                    // src = CPUREADNW(insn1->rn, insn1->mode);
                    // RETURN_IF(!tmpValue.hasValue());
                    if (!insn1->getOperand(bus, r, Bitmask(GetOperandOptions::Word), src))
                        return;
                    tmp = src;
                    if (tmp & 0x8000)
                    {
                        tmp >>= 1;
                        tmp |= 0x8000;
                    }
                    else
                        tmp >>= 1;
 
                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_C, src & 1);
                    PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
                    break;
                case 00063: /* ASL */
                    // src = CPUREADNW(insn1->rn, insn1->mode);
                    // RETURN_IF(!tmpValue.hasValue());
                    if (!insn1->getOperand(bus, r, Bitmask(GetOperandOptions::Word), src))
                        return;
                    tmp = src << 1;
                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_C, src & 0x8000);
                    PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
                    break;

                case 00064: /* MARK */
                    r[6] = r[7] + 2 * insnmark->nn;
                    r[7] = r[5];
                    r[5] = READ(r[6]);
                    r[6] += 2;
                    break;
                case 00067: /* SXT */
                    if (PSW_GET(PSW_N))
                    {
                        tmp = 0xFFFF;
                    }
                    else
                    {
                        tmp = 0;
                    }
                    CPUWRITEW(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_Z, !PSW_GET(PSW_N));
                    PSW_CLR(PSW_V);
                    break;
                default: /* 006500-006677, 007000-007777: unused */
                    TRCTrap(010, TRC_TRAP_ILL);
                    TRAP(illegalInstructionTrap);
                    break;
            }
            break;

        case 001: /* MOV */
            // tmp = CPUREADW(insn2->src_rn, insn2->src_mode);
            if (!insn2->getSourceOperand (bus, r, 
                Bitmask(GetOperandOptions::Word | 
                    GetOperandOptions::AutoIncr), tmp))
                return;

            CPUWRITEW(insn2->dst_rn, insn2->dst_mode, tmp);
            PSW_EQ(PSW_N, tmp & 0x8000);
            PSW_EQ(PSW_Z, !tmp);
            PSW_CLR(PSW_V);
            break;

        case 002: /* CMP */
            // src = CPUREADW(insn2->src_rn, insn2->src_mode);
            if (!insn2->getSourceOperand (bus, r, 
                Bitmask(GetOperandOptions::Word |
                    GetOperandOptions::AutoIncr), src))
                return;

            // dst = CPUREADW(insn2->dst_rn, insn2->dst_mode);
            if (!insn2->getDestOperand (bus, r, 
                Bitmask(GetOperandOptions::Word |
                    GetOperandOptions::AutoIncr), dst))
                return;

            tmp = src - dst;
            PSW_EQ(PSW_N, tmp & 0x8000);
            PSW_EQ(PSW_Z, !tmp);
            PSW_EQ(PSW_V, ((src & 0x8000) != (dst & 0x8000)) \
                && ((dst & 0x8000) == (tmp & 0x8000)));
            PSW_EQ(PSW_C, ((u32)src - (u32)dst) & 0x10000);
            break;

        case 003: /* BIT */
            // src = CPUREADW(insn2->src_rn, insn2->src_mode);
            if (!insn2->getSourceOperand (bus, r, 
                Bitmask(GetOperandOptions::Word |
                    GetOperandOptions::AutoIncr), src))
                return;

            // dst = CPUREADW(insn2->dst_rn, insn2->dst_mode);
            if (!insn2->getDestOperand (bus, r, 
                Bitmask(GetOperandOptions::Word |
                    GetOperandOptions::AutoIncr), dst))
                return;

            tmp = src & dst;
            PSW_EQ(PSW_N, tmp & 0x8000);
            PSW_EQ(PSW_Z, !tmp);
            PSW_CLR(PSW_V);
            break;

        case 004: /* BIC */
            // src = CPUREADW(insn2->src_rn, insn2->src_mode);
            if (!insn2->getSourceOperand (bus, r, 
                Bitmask(GetOperandOptions::Word |
                    GetOperandOptions::AutoIncr), src))
                return;

            // dst = CPUREADNW(insn2->dst_rn, insn2->dst_mode);
            // RETURN_IF(!tmpValue.hasValue());
            if (!insn2->getDestOperand (bus, r, 
                Bitmask(GetOperandOptions::Word), dst))
                return;

            tmp = ~src & dst;
            CPUWRITEW(insn2->dst_rn, insn2->dst_mode, tmp);
            PSW_EQ(PSW_N, tmp & 0x8000);
            PSW_EQ(PSW_Z, !tmp);
            PSW_CLR(PSW_V);
            break;

        case 005: /* BIS */
            // src = CPUREADW(insn2->src_rn, insn2->src_mode);
            if (!insn2->getSourceOperand (bus, r, 
                Bitmask(GetOperandOptions::Word |
                    GetOperandOptions::AutoIncr), src))
                return;

            // dst = CPUREADNW(insn2->dst_rn, insn2->dst_mode);
            if (!insn2->getDestOperand (bus, r, 
                Bitmask(GetOperandOptions::Word), dst))
               return;

            tmp = src | dst;
            CPUWRITEW(insn2->dst_rn, insn2->dst_mode, tmp);
            PSW_EQ(PSW_N, tmp & 0x8000);
            PSW_EQ(PSW_Z, !tmp);
            PSW_CLR(PSW_V);
            break;

        case 006: /* ADD */
            // src = CPUREADW(insn2->src_rn, insn2->src_mode);
            if (!insn2->getSourceOperand (bus, r, 
                Bitmask(GetOperandOptions::Word |
                    GetOperandOptions::AutoIncr), src))
                return;

            // dst = CPUREADNW(insn2->dst_rn, insn2->dst_mode);
            // RETURN_IF(!tmpValue.hasValue());
            if (!insn2->getDestOperand (bus, r, 
                Bitmask(GetOperandOptions::Word), dst))
                return;

            tmp = src + dst;
            CPUWRITEW(insn2->dst_rn, insn2->dst_mode, tmp);
            PSW_EQ(PSW_N, tmp & 0x8000);
            PSW_EQ(PSW_Z, !tmp);
            PSW_EQ(PSW_V, ((src & 0x8000) == (dst & 0x8000)) \
                && ((dst & 0x8000) != (tmp & 0x8000)));
            PSW_EQ(PSW_C, ((u32)src + (u32)dst) & 0x10000);
            break;

        case 007: /* 07 xx xx group */
            switch (insn >> 9)
            {
                case 0070: /* MUL */
                    dst = r[insnjsr->r];
                    src = CPUREADW(insnjsr->rn, insnjsr->mode);
                    tmps32 = (s32)(s16)dst * (s16)src;
                    r[insnjsr->r] = (u16)(tmps32 >> 16);
                    r[insnjsr->r | 1] = (u16)tmps32;
                    PSW_CLR(PSW_V);
                    PSW_EQ(PSW_N, tmps32 < 0);
                    PSW_EQ(PSW_Z, !tmps32);
                    PSW_EQ(PSW_C, (tmps32 >= 0x7FFF) || (tmps32 < -0x8000));
                    break;
                case 0071: /* DIV */
                    tmps32 = (r[insnjsr->r] << 16)
                        | r[insnjsr->r | 1];
                    src = CPUREADW(insnjsr->rn, insnjsr->mode);
                    if (src == 0)
                    {
                        PSW_SET(PSW_C);
                        PSW_SET(PSW_V);
                    }
                    else
                    {
                        s32 quot = tmps32 / (s16)src;
                        s32 rem = tmps32 % (s16)src;
                        PSW_CLR(PSW_C);
                        if ((s16)quot != quot)
                        {
                            PSW_SET(PSW_V);
                        }
                        else
                        {
                            r[insnjsr->r] = (u16)quot;
                            r[insnjsr->r | 1] = (u16)rem;
                            PSW_EQ(PSW_Z, !quot);
                            PSW_EQ(PSW_N, quot < 0);
                        }
                    }
                    break;
                case 0072: /* ASH */
                    dst = r[insnjsr->r];
                    src = CPUREADW(insnjsr->rn, insnjsr->mode);
                    if (src & 0x20)
                    { /* negative; right */
                        s16 stmp, stmp2;
                        src = (~src & 0x1F) + 1;
                        stmp = (s16)dst;
                        stmp2 = stmp >> (src - 1);
                        stmp >>= src;
                        tmp = (u16)stmp;
                        PSW_EQ(PSW_C, stmp2 & 1);
                        PSW_CLR(PSW_V);
                    }
                    else if ((src & 0x1F) == 0)
                    {
                        /* nothing */
                        PSW_CLR(PSW_V);
                        PSW_CLR(PSW_C);
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
                            PSW_EQ(PSW_V, !((tmp2 == 0) || (((tmp2 & mask) | ~mask) == 0xFFFF)));
                        }
                        else
                        {
                            PSW_CLR(PSW_V);
                        }
                        PSW_EQ(PSW_C, (dst << (src - 1)) & 0x8000);
                        if ((dst & 0x8000) != (tmp & 0x8000))
                        {
                            PSW_SET(PSW_V);
                        }
                    }
                    r[insnjsr->r] = tmp;
                    PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    break;
                case 0073: /* ASHC */
                    dst = r[insnjsr->r];
                    tmps32 = (r[insnjsr->r] << 16)
                        | r[insnjsr->r | 1];
                    src = CPUREADW(insnjsr->rn, insnjsr->mode);
                    if ((src & 0x3F) == 0x20)
                    { /* negative; 32 right */
                        PSW_EQ(PSW_C, tmps32 & 0x80000000);
                        PSW_CLR(PSW_V);
                        if (PSW_GET(PSW_C))
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
                        PSW_EQ(PSW_C, stmp2 & 1);
                    }
                    else if ((src & 0x1F) == 0)
                    {
                        /* nothing */
                        PSW_CLR(PSW_V);
                        PSW_CLR(PSW_C);
                    }
                    else
                    { /* positive, left */
                        s32 stmp2;
                        src &= 0x1F;
                        stmp2 = tmps32 << (src - 1);
                        tmps32 <<= src;
                        PSW_EQ(PSW_C, stmp2 & 0x80000000);
                        PSW_EQ(PSW_V, !!(dst & 0x8000)
                            != !!(tmps32 & 0x80000000));
                    }
                    r[insnjsr->r] = (u16)(tmps32 >> 16);
                    r[insnjsr->r | 1] = (u16)tmps32;
                    PSW_EQ(PSW_N, tmps32 & 0x80000000);
                    PSW_EQ(PSW_Z, !tmps32);
                    break;
                case 0074: /* XOR */
                    src = r[insnjsr->r];
                    dst = CPUREADNW(insnjsr->rn, insnjsr->mode);
                    RETURN_IF(!tmpValue.hasValue());
                    tmp = src ^ dst;
                    CPUWRITEW(insnjsr->rn, insnjsr->mode, tmp);
                    PSW_EQ(PSW_N, tmp & 0x8000);
                    PSW_EQ(PSW_Z, !tmp);
                    PSW_CLR(PSW_V);
                    break;
                case 0075: /* FIS instructions */
                    switch (insn >> 3)
                    {
#ifdef USE_FLOAT
                        case 007500: /* FADD */
                            f1._u32 = (READ(r[insnrts->rn] + 4) << 16)
                                | READ(r[insnrts->rn] + 6);
                            f2._u32 = (READ(r[insnrts->rn]) << 16)
                                | READ(r[insnrts->rn] + 2);
                            f3._f32 = f1._f32 + f2._f32;
                            /* TODO: result <= 2**-128 -> result = 0 */
                            /* TODO: implement traps */
                            WRITE(r[insnrts->rn] + 4,
                                (u16)(f3._u32 >> 16));
                            WRITE(r[insnrts->rn] + 6, (u16)f3._u32);
                            r[insnrts->rn] += 4;
                            PSW_EQ(PSW_N, f3._f32 < 0);
                            PSW_EQ(PSW_Z, f3._f32 == 0);
                            PSW_CLR(PSW_V);
                            PSW_CLR(PSW_C);
                            break;
                        case 007501: /* FSUB */
                            f1._u32 = (READ(r[insnrts->rn] + 4) << 16)
                                | READ(r[insnrts->rn] + 6);
                            f2._u32 = (READ(r[insnrts->rn]) << 16)
                                | READ(r[insnrts->rn] + 2);
                            f3._f32 = f1._f32 - f2._f32;
                            /* TODO: result <= 2**-128 -> result = 0 */
                            /* TODO: implement traps */
                            WRITE(r[insnrts->rn] + 4,
                                (u16)(f3._u32 >> 16));
                            WRITE(r[insnrts->rn] + 6, (u16)f3._u32);
                            r[insnrts->rn] += 4;
                            PSW_EQ(PSW_N, f3._f32 < 0);
                            PSW_EQ(PSW_Z, f3._f32 == 0);
                            PSW_CLR(PSW_V);
                            PSW_CLR(PSW_C);
                            break;
                        case 007502: /* FMUL */
                            f1._u32 = (READ(r[insnrts->rn] + 4) << 16)
                                | READ(r[insnrts->rn] + 6);
                            f2._u32 = (READ(r[insnrts->rn]) << 16)
                                | READ(r[insnrts->rn] + 2);
                            f3._f32 = f1._f32 * f2._f32;
                            /* TODO: result <= 2**-128 -> result = 0 */
                            /* TODO: implement traps */
                            WRITE(r[insnrts->rn] + 4,
                                (u16)(f3._u32 >> 16));
                            WRITE(r[insnrts->rn] + 6, (u16)f3._u32);
                            r[insnrts->rn] += 4;
                            PSW_EQ(PSW_N, f3._f32 < 0);
                            PSW_EQ(PSW_Z, f3._f32 == 0);
                            PSW_CLR(PSW_V);
                            PSW_CLR(PSW_C);
                            break;
                        case 007503: /* FDIV */
                            f1._u32 = (READ(r[insnrts->rn] + 4) << 16)
                                | READ(r[insnrts->rn] + 6);
                            f2._u32 = (READ(r[insnrts->rn]) << 16)
                                | READ(r[insnrts->rn] + 2);
                            if (f2._f32 != 0)
                            {
                                f3._f32 = f1._f32 / f2._f32;
                                /* TODO: result <= 2**-128 -> result = 0 */
                                /* TODO: implement traps */
                                WRITE(r[insnrts->rn] + 4,
                                    (u16)(f3._u32 >> 16));
                                WRITE(r[insnrts->rn] + 6,
                                    (u16)f3._u32);
                                PSW_EQ(PSW_N, f3._f32 < 0);
                                PSW_EQ(PSW_Z, f3._f32 == 0);
                                PSW_CLR(PSW_V);
                                PSW_CLR(PSW_C);
                            }
                            r[insnrts->rn] += 4;
                            break;
#endif
                        default:
                            /* 075040-076777: unused */
                            TRCTrap(010, TRC_TRAP_ILL);
                            TRAP(illegalInstructionTrap);
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
                    TRCTrap(010, TRC_TRAP_ILL);
                    TRAP(illegalInstructionTrap);
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
                    if (!PSW_GET(PSW_N))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 01004: /* BMI */
                case 01005:
                case 01006:
                case 01007:
                    if (PSW_GET(PSW_N))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 01010: /* BHI */
                case 01011:
                case 01012:
                case 01013:
                    if (!PSW_GET(PSW_C) && !PSW_GET(PSW_Z))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 01014: /* BLOS */
                case 01015:
                case 01016:
                case 01017:
                    if (PSW_GET(PSW_C) || PSW_GET(PSW_Z))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 01020: /* BVC */
                case 01021:
                case 01022:
                case 01023:
                    if (!PSW_GET(PSW_V))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 01024: /* BVS */
                case 01025:
                case 01026:
                case 01027:
                    if (PSW_GET(PSW_V))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 01030: /* BCC */
                case 01031:
                case 01032:
                case 01033:
                    if (!PSW_GET(PSW_C))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 01034: /* BCS */
                case 01035:
                case 01036:
                case 01037:
                    if (PSW_GET(PSW_C))
                    {
                        r[7] += (s16)((s8)insnbr->offset) * 2;
                    }
                    break;
                case 01040: /* EMT */
                case 01041:
                case 01042:
                case 01043:
                    TRCTrap(030, TRC_TRAP);
                    TRAP(EMT);
                    break;
                case 01044: /* TRAP */
                case 01045:
                case 01046:
                case 01047:
                    TRCTrap(034, TRC_TRAP);
                    TRAP(TRP);
                    break;

                case 01050: /* CLRB */
                    CPUWRITEB(insn1->rn, insn1->mode, 0);
                    PSW_CLR(PSW_N | PSW_V | PSW_C);
                    PSW_SET(PSW_Z);
                    break;

                case 01051: /* COMB */
                    // tmp = CPUREADNB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, Bitmask(GetOperandOptions::Byte), tmp))
                        return;

                    tmp = ~tmp;
                    CPUWRITEB(insn1->rn, insn1->mode, tmp);
                    PSW_CLR(PSW_V);
                    PSW_SET(PSW_C);
                    PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !((u8)tmp));
                    break;

                case 01052: /* INCB */
                    // src = CPUREADNB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, Bitmask(GetOperandOptions::Byte), src))
                        return;

                    tmp = (u8)(src + 1);
                    CPUWRITEB(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_V, src == 000177)
                        PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !tmp);
                    break;

                case 01053: /* DECB */
                    // src = CPUREADNB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, Bitmask(GetOperandOptions::Byte), src))
                        return;

                    tmp = (u8)(src - 1);
                    CPUWRITEB(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_V, src == 0000200)
                        PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !tmp);
                    break;

                case 01054: /* NEGB */
                    // tmp = CPUREADNB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, Bitmask(GetOperandOptions::Byte), tmp))
                        return;

                    if (tmp != 0200)
                    {
                        tmp = -tmp;
                    }
                    CPUWRITEB(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_V, tmp == 0200)
                        PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !tmp);
                    PSW_EQ(PSW_C, tmp);
                    break;

                case 01055: /* ADCB */
                    // src = CPUREADNB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, Bitmask(GetOperandOptions::Byte), src))
                        return;

                    tmp2 = PSW_GET(PSW_C) ? 1 : 0;
                    tmp = (u8)(src + tmp2);
                    CPUWRITEB(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_V, src == 0177 && PSW_GET(PSW_C));
                    PSW_EQ(PSW_C, src == 0377 && PSW_GET(PSW_C));
                    PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !tmp);
                    break;

                case 01056: /* SBCB */
                    // src = CPUREADNB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, Bitmask(GetOperandOptions::Byte), src))
                        return;

                    tmp2 = PSW_GET(PSW_C) ? 1 : 0;
                    tmp = (u8)(src - tmp2);
                    CPUWRITEB(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_V, src == 0200);
                    PSW_EQ(PSW_C, !src && PSW_GET(PSW_C));
                    PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !tmp);
                    break;

                case 01057: /* TSTB */
                    // tmp = CPUREADB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, 
                        Bitmask(GetOperandOptions::Byte |
                            GetOperandOptions::AutoIncr), tmp))
                        return;

                    PSW_CLR(PSW_V);
                    PSW_CLR(PSW_C);
                    PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !tmp);
                    break;

                case 01060: /* RORB */
                    // src = CPUREADNB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, Bitmask(GetOperandOptions::Byte), src))
                        return;

                    tmp2 = PSW_GET(PSW_C);
                    tmp = src >> 1;
                    if (tmp2)
                    {
                        tmp |= 0x80;
                    }
                    CPUWRITEB(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_C, src & 0x01);
                    PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !tmp);
                    PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
                    break;

                case 01061: /* ROLB */
                    // src = CPUREADNB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, Bitmask(GetOperandOptions::Byte), src))
                        return;

                    tmp2 = PSW_GET(PSW_C);
                    tmp = (u8)(src << 1);
                    if (tmp2)
                    {
                        tmp |= 0x01;
                    }
                    CPUWRITEB(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_C, src & 0x80);
                    PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !tmp);
                    PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
                    break;

                case 01062: /* ASRB */
                    // src = CPUREADNB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, Bitmask(GetOperandOptions::Byte), src))
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
                    CPUWRITEB(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_C, src & 1);
                    PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !tmp);
                    PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
                    break;

                case 01063: /* ASLB */
                    // src = CPUREADNB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, Bitmask(GetOperandOptions::Byte), src))
                        return;

                    tmp = (u8)(src << 1);
                    CPUWRITEB(insn1->rn, insn1->mode, tmp);
                    PSW_EQ(PSW_C, src & 0x80);
                    PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !tmp);
                    PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
                    break;

                case 01064: 
                    // MTPS
                    // Note that the T bit cannot be set with this instruction
                    // tmp = CPUREADB(insn1->rn, insn1->mode);
                    if (!insn1->getOperand (bus, r, 
                        Bitmask(GetOperandOptions::Byte |
                            GetOperandOptions::AutoIncr), tmp))
                        return;

                    psw = (psw & PSW_T) | (tmp & ~PSW_T);
                    break;

                case 01067: /* MFPS */
                    tmp = (u8)psw;
                    if (insn1->mode == 0)
                    {
                        r[insn1->rn] = (s8)psw;
                    }
                    else
                    {
                        CPUWRITEB(insn1->rn, insn1->mode, tmp);
                    }
                    PSW_EQ(PSW_N, tmp & 0x80);
                    PSW_EQ(PSW_Z, !(tmp & 0xFF));
                    PSW_CLR(PSW_V);
                    break;

                default:
                    /* unused */
                    TRCTrap(010, TRC_TRAP_ILL);
                    TRAP(illegalInstructionTrap);
                    break;
            }
            break;

        case 011: /* MOVB */
            // tmp = CPUREADB(insn2->src_rn, insn2->src_mode);
            if (!insn2->getSourceOperand (bus, r, 
                Bitmask(GetOperandOptions::Byte |
                    GetOperandOptions::AutoIncr),tmp))
                return;
            tmp = (s8)tmp;
            if (insn2->dst_mode == 0)
            {
                r[insn2->dst_rn] = tmp;
            }
            else
            {
                CPUWRITEB(insn2->dst_rn, insn2->dst_mode, tmp);
            }
            PSW_EQ(PSW_N, tmp & 0x80);
            PSW_EQ(PSW_Z, !tmp);
            PSW_CLR(PSW_V);
            break;

        case 012: /* CMPB */
            src = CPUREADB(insn2->src_rn, insn2->src_mode);
            dst = CPUREADB(insn2->dst_rn, insn2->dst_mode);
            tmp = (u8)(src - dst);
            PSW_EQ(PSW_N, tmp & 0x80);
            PSW_EQ(PSW_Z, !tmp);
            PSW_EQ(PSW_V, ((src & 0x80) != (dst & 0x80)) \
                && ((dst & 0x80) == (tmp & 0x80)));
            PSW_EQ(PSW_C, (src - dst) & 0x100);
            break;

        case 013: /* BITB */
            src = CPUREADB(insn2->src_rn, insn2->src_mode);
            dst = CPUREADB(insn2->dst_rn, insn2->dst_mode);
            tmp = src & dst;
            PSW_EQ(PSW_N, tmp & 0x80);
            PSW_EQ(PSW_Z, !tmp);
            PSW_CLR(PSW_V);
            break;

        case 014: /* BICB */
            src = CPUREADB(insn2->src_rn, insn2->src_mode);
            dst = CPUREADNB(insn2->dst_rn, insn2->dst_mode);
            tmp = (u8)(~src & dst);
            CPUWRITEB(insn2->dst_rn, insn2->dst_mode, tmp);
            PSW_EQ(PSW_N, tmp & 0x80);
            PSW_EQ(PSW_Z, !tmp);
            PSW_CLR(PSW_V);
            break;

        case 015: /* BISB */
            src = CPUREADB(insn2->src_rn, insn2->src_mode);
            dst = CPUREADNB(insn2->dst_rn, insn2->dst_mode);
            tmp = src | dst;
            CPUWRITEB(insn2->dst_rn, insn2->dst_mode, tmp);
            PSW_EQ(PSW_N, tmp & 0x80);
            PSW_EQ(PSW_Z, !tmp);
            PSW_CLR(PSW_V);
            break;

        case 016: /* SUB */
            // src = CPUREADW(insn2->src_rn, insn2->src_mode);
            if (!insn2->getSourceOperand (bus, r, 
                Bitmask(GetOperandOptions::Word | 
                    GetOperandOptions::AutoIncr), src))
                return;

            // dst = CPUREADNW(insn2->dst_rn, insn2->dst_mode);
            // RETURN_IF(!tmpValue.hasValue());
            if (!insn2->getDestOperand (bus, r, Bitmask(GetOperandOptions::Word), dst))
                return;

            tmp = dst - src;
            CPUWRITEW(insn2->dst_rn, insn2->dst_mode, tmp);
            PSW_EQ(PSW_N, tmp & 0x8000);
            PSW_EQ(PSW_Z, !tmp);
            PSW_EQ(PSW_V, ((src & 0x8000) != (dst & 0x8000)) \
                && ((src & 0x8000) == (tmp & 0x8000)));
            PSW_EQ(PSW_C, ((u32)dst - (u32)src) & 0x10000);
            break;
        default: /* unused */
            TRCTrap(010, TRC_TRAP_ILL);
            TRAP(illegalInstructionTrap);
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

void KD11CPU::handleTraps(QBUS* bus)
{
    InterruptRequest intrptReq;
	u16 trapToProcess{0};
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
    // isn't halted. This is the most common
	// case so check this as first.
    if ((!trap_ && !bus->intrptReqAvailable()) || runState == STATE_HALT)
        return;

    // Traps have the highest priority, so first check if there is a trap
    // to handle
    if (trap_)
    {
        trapToProcess = trap_->vector();
        trap_ = nullptr;
    }
    else if (bus->intrptPriority() > cpuPriority())
    {
        if (bus->getIntrptReq(intrptReq))
            trapToProcess = intrptReq.vector();
        else
            return;
    }
    else return;


	TRCCPUEvent(TRC_CPU_TRAP, trapToProcess);

	// Save PC and PSW on the stack. Adressing the stack could result in a
	// bus time out. In that case the CPU is halted.
	// ToDo: Remove code duplication
	r[6] -= 2;
	if (!WRITE(r[6], psw))
	{
		TRCCPUEvent(TRC_CPU_DBLBUS, r[6]);
        // ToDo: All interrupts should be cleared?
		trap_ = nullptr;
		runState = STATE_HALT;
		return;
	}

	r[6] -= 2;
	if (!WRITE(r[6], r[7]))
	{
		TRCCPUEvent(TRC_CPU_DBLBUS, r[6]);
		trap_ = nullptr;
		runState = STATE_HALT;
		return;
	}

	// Read new PC and PSW from the trap vector. These read's could also
	// result in a bus time out.
    tmpValue = READ(trapToProcess);
    r[7] = tmpValue.valueOr(0);
    if (!tmpValue.hasValue())
	{
		TRCCPUEvent(TRC_CPU_DBLBUS, trapToProcess);
		trap_ = nullptr;
		runState = STATE_HALT;
		return;
	}

    tmpValue = READ(trapToProcess + 2);
    psw = tmpValue.valueOr(0);
    if (!tmpValue.hasValue())
	{
		TRCCPUEvent(TRC_CPU_DBLBUS, trapToProcess + 2);
		trap_ = nullptr;
		runState = STATE_HALT;
		return;
	}

	/* resume execution if in WAIT state */
	if(runState == STATE_WAIT) 
	{
		TRCCPUEvent(TRC_CPU_RUN, r[7]);
		runState = STATE_RUN;
	}
}

void KD11::step(QBUS* bus)
{
	switch(cpu_.runState) 
	{
		case STATE_HALT:
			odt.step (bus);
			break;

		case STATE_RUN:
			cpu_.step (bus);
			if (cpu_.runState == STATE_HALT)
				// Reset the ODT state to ODT_STATE_INIT
				odt.reset();
			break;

		case STATE_WAIT:
			cpu_.handleTraps (bus);
			break;
	}
}

// Generate the given trap using the interrupt request mechanism
void KD11CPU::setTrap(QBUS *bus, InterruptRequest const *trap)
{
    trap_ = trap;
}

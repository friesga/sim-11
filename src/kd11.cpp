#include <string.h>
#include <stdlib.h>

#include "lsi11.h"
#include "trace.h"

#define USE_FLOAT

/* ODT states */
#define	ODT_STATE_INIT		0
#define	ODT_STATE_WAIT		1
#define	ODT_STATE_ADDR		2
#define	ODT_STATE_REG		3
#define	ODT_STATE_REG_WAIT	4
#define	ODT_STATE_VAL		5
#define	ODT_STATE_REG_VAL	6
#define	ODT_STATE_WR		7

/* CPU states */
#define	STATE_HALT		0
#define	STATE_RUN		1
#define	STATE_WAIT		2
#define	STATE_INHIBIT_TRACE	3

#define	PSW_C			_BV(0)
#define	PSW_V			_BV(1)
#define	PSW_Z			_BV(2)
#define	PSW_N			_BV(3)
#define	PSW_T			_BV(4)
#define	PSW_PRIO		_BV(7)

#define	PSW_GET(x)		(((psw) & (x)) ? 1 : 0)
#define	PSW_SET(x)		((psw) |= (x))
#define	PSW_CLR(x)		((psw) &= ~(x))
#define	PSW_EQ(x, v) { \
	if(v) { \
		PSW_SET(x); \
	} else { \
		PSW_CLR(x); \
	} \
}

#define	TRAP(n)		setTrap(n)

// (Try to) determine the byte order. To that end gcc provides the __BYTE__ORDER__
// definition. Msvc has no such definition but we can safely assume that all
// win32 machines are little endian.
#if _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
/* little endian host */

typedef struct {
	u16	rn:3;
	u16	mode:3;
	u16	opcode:10;
} KD11INSN1;

typedef struct {
	u16	dst_rn:3;
	u16	dst_mode:3;
	u16	src_rn:3;
	u16	src_mode:3;
	u16	opcode:4;
} KD11INSN2;

typedef struct {
	u16	offset:8;
	u16	opcode:8;
} KD11INSNBR;

typedef struct {
	u16	rn:3;
	u16	mode:3;
	u16	r:3;
	u16	opcode:7;
} KD11INSNJSR;

typedef struct {
	u16	rn:3;
	u16	opcode:13;
} KD11INSNRTS;

typedef struct {
	u16	nn:6;
	u16	opcode:10;
} KD11INSNMARK;

typedef struct {
	u16	offset:6;
	u16	rn:3;
	u16	opcode:7;
} KD11INSNSOB;
#else
/* big endian host */
typedef struct {
	u16	opcode:10;
	u16	mode:3;
	u16	rn:3;
} KD11INSN1;

typedef struct {
	u16	opcode:4;
	u16	src_mode:3;
	u16	src_rn:3;
	u16	dst_mode:3;
	u16	dst_rn:3;
} KD11INSN2;

typedef struct {
	u16	opcode:8;
	u16	offset:8;
} KD11INSNBR;

typedef struct {
	u16	opcode:7;
	u16	r:3;
	u16	mode:3;
	u16	rn:3;
} KD11INSNJSR;

typedef struct {
	u16	opcode:13;
	u16	rn:3;
} KD11INSNRTS;

typedef struct {
	u16	opcode:10;
	u16	nn:6;
} KD11INSNMARK;

typedef struct {
	u16	opcode:7;
	u16	rn:3;
	u16	offset:6;
} KD11INSNSOB;
#endif // _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

// Constructor
// ToDo: Reinitialize K11ODT
KD11::KD11()
	:
	r{0}, 
	psw{0},
	runState{0},
	trap{0}
{}

void KD11::reset()
{
	r[7] = 0173000;
	psw = 0;
	trap = 0;
	runState = STATE_HALT;
	odt.state = ODT_STATE_INIT;
}

#define	READ(addr)			(bus->read((addr)))
#define	WRITE(addr, val)	(bus->write((addr), (val)))
#define	CHECK()			{ \
	if(trap && trap <= 010) \
		return; \
	}

void KD11::KD11ODTClear()
{
	odt.buf_sz = 0;
}

void KD11::KD11ODTWrite(u8 c)
{
	odt.buf[odt.buf_sz++] = c;
	odt.buf_r = 0;
}

void KD11::KD11ODTWriteOctal(u16 val)
{
	int i;
	odt.buf[odt.buf_sz++] = ((val >> 15) & 0x7) + '0';
	for(i = 0; i < 5; i++) {
		odt.buf[odt.buf_sz++] = ((val >> 12) & 7) + '0';
		val <<= 3;
	}
}

void KD11::KD11ODTInputError()
{
	odt.state = ODT_STATE_WR;
	odt.next = ODT_STATE_WAIT;
	odt.buf[0] = '?';
	odt.buf[1] = '\r';
	odt.buf[2] = '\n';
	odt.buf[3] = '@';
	odt.buf_r = 0;
	odt.buf_sz = 4;
}

void KD11::KD11ODTStep(QBUS* bus)
{
	/* odt */
	// KD11ODT* odt = &odt;
	switch(odt.state) {
		case ODT_STATE_INIT:
			KD11ODTClear();
			KD11ODTWrite('\r');
			KD11ODTWrite('\n');
			KD11ODTWriteOctal(r[7]);
			KD11ODTWrite('\r');
			KD11ODTWrite('\n');
			KD11ODTWrite('@');
			odt.next = ODT_STATE_WAIT;
			odt.state = ODT_STATE_WR;
			break;
		case ODT_STATE_WAIT:
			if(READ(0177560) & 0x80) { /* ch available */
				u16 c = READ(0177562);
				WRITE(0177566, (u8) c);
				switch((u8) c) {
					case '$':
					case 'R':
						odt.state = ODT_STATE_REG;
						break;
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
						odt.state = ODT_STATE_ADDR;
						odt.addr = ((u8) c) - '0';
						break;
					case 'G':
						KD11ODTClear();
						KD11ODTWrite('\r');
						KD11ODTWrite('\n');
						KD11ODTWrite('@');
						odt.next = ODT_STATE_WAIT;
						odt.state = ODT_STATE_WR;
						break;
					case 'P':
						odt.state = ODT_STATE_INIT;
						runState = STATE_RUN;
						TRCCPUEvent(TRC_CPU_ODT_P, r[7]);
						break;
					default:
						KD11ODTInputError();
						break;
				}
			}
			break;
		case ODT_STATE_WR:
			if(READ(0177564) & 0x80) {
				WRITE(0177566, odt.buf[odt.buf_r++]);
				if(odt.buf_r == odt.buf_sz) {
					odt.state = odt.next;
				}
			}
			break;
		case ODT_STATE_ADDR:
			if(READ(0177560) & 0x80) { /* ch available */
				u16 ch = READ(0177562);
				u8 c = (u8) ch;
				WRITE(0177566, c);
				if((u8) c == '/') { /* delimit */
					u16 val = READ(odt.addr);
					KD11ODTClear();
					KD11ODTWriteOctal(val);
					KD11ODTWrite(' ');
					odt.next = ODT_STATE_VAL;
					odt.state = ODT_STATE_WR;
					odt.input = 0;
				} else if(c >= '0' && c <= '7') {
					odt.addr <<= 3;
					odt.addr |= c - '0';
				} else if(c == 'G') {
					odt.state = ODT_STATE_INIT;
					r[7] = odt.addr;
					runState = STATE_RUN;
					TRCCPUEvent(TRC_CPU_ODT_G, odt.addr);
				} else {
					KD11ODTInputError();
				}
			}
			break;
		case ODT_STATE_REG:
			if(READ(0177560) & 0x80) { /* ch available */
				u16 ch = READ(0177562);
				u8 c = (u8) ch;
				WRITE(0177566, c);
				odt.state = ODT_STATE_REG_WAIT;
				if(c >= '0' && c <= '7') {
					odt.addr = c - '0';
				} else if(c == 'S') {
					odt.addr = 8;
				} else {
					KD11ODTInputError();
				}
			}
			break;
		case ODT_STATE_VAL:
			if(READ(0177560) & 0x80) { /* ch available */
				u16 ch = READ(0177562);
				u8 c = (u8) ch;
				WRITE(0177566, c);
				if(c == '\r' || c == '\n') {
					if(odt.input) {
						WRITE(odt.addr, odt.val);
					}
				} else if(c >= '0' && c <= '7') {
					odt.val <<= 3;
					odt.val |= c - '0';
					odt.input = 1;
				} else {
					KD11ODTInputError();
				}
				if(c == '\r') {
					KD11ODTClear();
					KD11ODTWrite('\r');
					KD11ODTWrite('\n');
					KD11ODTWrite('@');
					odt.state = ODT_STATE_WR;
					odt.next = ODT_STATE_WAIT;
				} else if(c == '\n') {
					u16 val;

					odt.addr += 2;
					odt.val = 0;
					val = READ(odt.addr);

					KD11ODTClear();
					KD11ODTWrite('\r');
					KD11ODTWrite('\n');
					KD11ODTWriteOctal(odt.addr);
					KD11ODTWrite('/');
					KD11ODTWriteOctal(val);
					KD11ODTWrite(' ');

					odt.next = ODT_STATE_VAL;
					odt.state = ODT_STATE_WR;
					odt.input = 0;
				}
			}
			break;
		case ODT_STATE_REG_WAIT:
			if(READ(0177560) & 0x80) { /* ch available */
				u16 ch = READ(0177562);
				u8 c = (u8) ch;
				WRITE(0177566, c);
				if(c == '/') {
					u16 val;
					if(odt.addr < 8) {
						val = r[odt.addr];
					} else {
						val = psw;
					}
					KD11ODTClear();
					KD11ODTWriteOctal(val);
					KD11ODTWrite(' ');
					odt.val = 0;
					odt.next = ODT_STATE_REG_VAL;
					odt.state = ODT_STATE_WR;
					odt.input = 0;
				} else {
					KD11ODTInputError();
				}
			}
			break;
		case ODT_STATE_REG_VAL:
			if(READ(0177560) & 0x80) { /* ch available */
				u16 ch = READ(0177562);
				u8 c = (u8) ch;
				WRITE(0177566, c);
				if(c == '\r' || c == '\n') {
					if(odt.input) {
						if(odt.addr == 8) {
							psw = odt.val;
						} else {
							r[odt.addr] = odt.val;
						}
					}
				} else if(c >= '0' && c <= '7') {
					odt.val <<= 3;
					odt.val |= c - '0';
					odt.input = 1;
				} else {
					KD11ODTInputError();
				}
				if(c == '\r' || (c == '\n' && odt.addr == 7)) {
					KD11ODTClear();
					KD11ODTWrite('\r');
					KD11ODTWrite('\n');
					KD11ODTWrite('@');
					odt.state = ODT_STATE_WR;
					odt.next = ODT_STATE_WAIT;
				} else if(c == '\n') {
					u16 val;

					odt.addr++;
					odt.val = 0;
					val = r[odt.addr];

					KD11ODTClear();
					KD11ODTWrite('\r');
					KD11ODTWrite('R');
					KD11ODTWrite(odt.addr + '0');
					KD11ODTWrite('/');
					KD11ODTWriteOctal(val);
					KD11ODTWrite(' ');

					odt.next = ODT_STATE_REG_VAL;
					odt.state = ODT_STATE_WR;
					odt.input = 0;
				}
			}
			break;
	}
}

#define	CHECKREAD()		{ \
	if(trap && trap <= 010) \
		return 0; \
	}

u16 KD11::KD11CPUReadW(QBUS* bus, u16 dst, u16 mode, int inc)
{
	u16 addr;
	switch(mode) {
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
			CHECKREAD();
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
			CHECKREAD();
			return READ(addr);
		case 6: /* Index */
			addr = READ(r[7]);
			CHECKREAD();
			if(inc) {
				r[7] += 2;
			} else if(dst == 7) {
				addr += 2;
			}
			addr += r[dst];
			return READ(addr);
		case 7: /* Index indirect */
			addr = READ(r[7]);
			CHECKREAD();
			if(inc) {
				r[7] += 2;
			} else if(dst == 7) {
				addr += 2;
			}
			addr += r[dst];
			addr = READ(addr);
			CHECKREAD();
			return READ(addr);
		default:
			return 0;
	}
}

#define	READ8(addr) (((addr) & 1) ? (u8) (READ((addr) & 0xFFFE) >> 8) : \
				((u8) READ(addr & 0xFFFE)))

u8 KD11::KD11CPUReadB(QBUS* bus, u16 dst, u16 mode, int inc)
{
	u16 addr;
	switch(mode) {
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
			CHECKREAD();
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
			CHECKREAD();
			return READ8(addr);
		case 6: /* Index */
			addr = READ(r[7]);
			CHECKREAD();
			if(inc) {
				r[7] += 2;
			} else if(dst == 7) {
				addr += 2;
			}
			addr += r[dst];
			return READ8(addr);
		case 7: /* Index indirect */
			addr = READ(r[7]);
			CHECKREAD();
			if(inc) {
				r[7] += 2;
			} else if(dst == 7) {
				addr += 2;
			}
			addr += r[dst];
			addr = READ(addr);
			CHECKREAD();
			return READ8(addr);
		default:
			return 0;
	}
}

void KD11::KD11CPUWriteW(QBUS* bus, u16 dst, u16 mode, u16 val)
{
	u16 addr;
	switch(mode) {
		case 0: /* Register */
			r[dst] = val;
			break;
		case 1: /* Register indirect */
			WRITE(r[dst], val);
			break;
		case 2: /* Autoincrement */
			r[dst] &= 0xFFFE;
			addr = r[dst];
			r[dst] += 2;
			WRITE(addr, val);
			break;
		case 3: /* Autoincrement indirect */
			r[dst] &= 0xFFFE;
			addr = r[dst];
			r[dst] += 2;
			addr = READ(addr);
			CHECK();
			WRITE(addr, val);
			break;
		case 4: /* Autodecrement */
			r[dst] &= 0xFFFE;
			r[dst] -= 2;
			addr = r[dst];
			WRITE(addr, val);
			break;
		case 5: /* Autodecrement indirect */
			r[dst] &= 0xFFFE;
			r[dst] -= 2;
			addr = r[dst];
			addr = READ(addr);
			CHECK();
			WRITE(addr, val);
			break;
		case 6: /* Index */
			addr = READ(r[7]);
			CHECK();
			r[7] += 2;
			addr += r[dst];
			WRITE(addr, val);
			break;
		case 7: /* Index indirect */
			addr = READ(r[7]);
			CHECK();
			r[7] += 2;
			addr += r[dst];
			addr = READ(addr);
			CHECK();
			WRITE(addr, val);
			break;
	}
}

#define	WRITE8(addr, val)	{ \
	u16 aaddr = addr & 0xFFFE; \
	u16 tmp = READ(aaddr); \
	if(addr & 1) { \
		tmp = (tmp & 0x00FF) | (val << 8); \
	} else { \
		tmp = (tmp & 0xFF00) | val; \
	} \
	WRITE(aaddr, tmp); \
}

void KD11::KD11CPUWriteB(QBUS* bus, u16 dst, u16 mode, u8 val)
{
	u16 addr;
	switch(mode) {
		case 0: /* Register */
			r[dst] = (r[dst] & 0xFF00) | val;
			break;
		case 1: /* Register deferred */
			WRITE8(r[dst], val);
			break;
		case 2: /* Autoincrement */
			addr = r[dst];
			if(dst == 6 || dst == 7) {
				r[dst] += 2;
			} else {
				r[dst]++;
			}
			WRITE8(addr, val);
			break;
		case 3: /* Autoincrement deferred */
			addr = r[dst];
			r[dst] += 2;
			addr = READ(addr);
			CHECK();
			WRITE8(addr, val);
			break;
		case 4: /* Autodecrement */
			if(dst == 6 || dst == 7) {
				r[dst] -= 2;
			} else {
				r[dst]--;
			}
			addr = r[dst];
			WRITE8(addr, val);
			break;
		case 5: /* Autodecrement deferred */
			r[dst] -= 2;
			addr = r[dst];
			addr = READ(addr);
			CHECK();
			WRITE8(addr, val);
			break;
		case 6: /* Index */
			addr = READ(r[7]);
			CHECK();
			r[7] += 2;
			addr += r[dst];
			WRITE8(addr, val);
			break;
		case 7: /* Index deferred */
			addr = READ(r[7]);
			CHECK();
			r[7] += 2;
			addr += r[dst];
			addr = READ(addr);
			CHECK();
			WRITE8(addr, val);
			break;
	}
}

u16 KD11::KD11CPUGetAddr(QBUS* bus, u16 dst, u16 mode)
{
	u16 addr;
	switch(mode) {
		case 0: /* Register */
			TRCTrap(4, TRC_TRAP_RADDR);
			TRAP(4); /* illegal instruction */
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
			CHECKREAD();
			r[7] += 2;
			addr += r[dst];
			return addr;
		case 7: /* Index indirect */
			addr = READ(r[7]);
			CHECKREAD();
			r[7] += 2;
			addr += r[dst];
			addr = READ(addr);
			return addr;
		default:
			return 0;
	}
}

#define	CPUREADW(rn, mode)	KD11CPUReadW(bus, rn, mode, 1); \
				CHECK()
#define	CPUREADB(rn, mode)	KD11CPUReadB(bus, rn, mode, 1); \
				CHECK()
#define	CPUREADNW(rn, mode)	KD11CPUReadW(bus, rn, mode, 0); \
				CHECK()
#define	CPUREADNB(rn, mode)	KD11CPUReadB(bus, rn, mode, 0); \
				CHECK()
#define CPUWRITEW(rn, mode, v)	KD11CPUWriteW(bus, rn, mode, v); \
				CHECK()
#define CPUWRITEB(rn, mode, v)	KD11CPUWriteB(bus, rn, mode, (u8) v); \
				CHECK()

typedef union {
	float	f32;
	u32		u32;
} FLOAT;

void KD11::KD11CPUStep(QBUS* bus)
{
	u16 tmp, tmp2;
	u16 src, dst;
	s32 tmps32;
#ifdef USE_FLOAT
	FLOAT f1, f2, f3;
	u8 unknown = 0;
#endif

	u16 insn = READ(r[7]);
	KD11INSN1* insn1 = (KD11INSN1*) &insn;
	KD11INSN2* insn2 = (KD11INSN2*) &insn;
	KD11INSNBR* insnbr = (KD11INSNBR*) &insn;
	KD11INSNJSR* insnjsr = (KD11INSNJSR*) &insn;
	KD11INSNRTS* insnrts = (KD11INSNRTS*) &insn;
	KD11INSNMARK* insnmark = (KD11INSNMARK*) &insn;
	KD11INSNSOB* insnsob = (KD11INSNSOB*) &insn;

	r[7] += 2;

	CHECK();

	/* single operand instructions */
switch(insn >> 12) {
		case 000: /* 00 xx xx group */
			switch(insn >> 6) {
				case 00000: /* 00 00 xx group */
					switch(insn) {
						case 0000000: /* HALT */
							TRCCPUEvent(TRC_CPU_HALT, r[7]);
							runState = STATE_HALT;
							odt.state = ODT_STATE_INIT;
							break;
						case 0000001: /* WAIT */
							TRCCPUEvent(TRC_CPU_WAIT, r[7]);
							runState = STATE_WAIT;
							break;
						case 0000002: /* RTI */
							r[7] = READ(r[6]);
							r[6] += 2;
							CHECK();
							psw = READ(r[6]);
							r[6] += 2;
							CHECK();
							break;
						case 0000003: /* BPT */
							TRCTrap(014, TRC_TRAP);
							TRAP(014);
							break;
						case 0000004: /* IOT */
							TRCTrap(020, TRC_TRAP);
							TRAP(020);
							break;
						case 0000005: /* RESET */
							bus->reset();
							break;
						case 0000006: /* RTT */
							r[7] = READ(r[6]);
							r[6] += 2;
							CHECK();
							psw = READ(r[6]);
							r[6] += 2;
							CHECK();
							runState = STATE_INHIBIT_TRACE;
							break;
						default: /* 00 00 07 - 00 00 77 */
							/* unused opcodes */
							TRCTrap(010, TRC_TRAP_ILL);
							TRAP(010);
							break;
					}
					break;
				case 00001: /* JMP */
					tmp = KD11CPUGetAddr(bus, insn1->rn, insn1->mode);
					CHECK();
					r[7] = tmp;
					break;
				case 00002: /* 00 02 xx group */
					/* mask=177740: CLN/CLZ/CLV/CLC/CCC/SEN/SEZ/SEV/SEC/SCC */
					if((insn & 0177770) == 0000200) {
						/* RTS */
						r[7] = r[insnrts->rn];
						r[insnrts->rn] = READ(r[6]);
						r[6] += 2;
					} else if((insn & 0177740) == 0000240) {
						tmp = insn & 017;
						if(insn & 020) {
							psw |= tmp;
						} else {
							psw &= ~tmp;
						}
					} else {
						/* 00 02 10 - 00 02 27: unused */
						TRCTrap(010, TRC_TRAP_ILL);
						TRAP(010);
					}
					break;
				case 00003: /* SWAB */
					tmp = CPUREADNW(insn1->rn, insn1->mode);
					tmp = ((tmp & 0x00FF) << 8) | ((tmp >> 8) & 0xFF);
					CPUWRITEW(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !((u8) tmp));
					PSW_CLR(PSW_V);
					PSW_CLR(PSW_C);
					break;
				case 00004: /* BR */
				case 00005:
				case 00006:
				case 00007:
					r[7] += (s16) ((s8) insnbr->offset) * 2;
					break;
				case 00010: /* BNE */
				case 00011:
				case 00012:
				case 00013:
					if(!PSW_GET(PSW_Z)) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 00014: /* BEQ */
				case 00015:
				case 00016:
				case 00017:
					if(PSW_GET(PSW_Z)) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 00020: /* BGE */
				case 00021:
				case 00022:
				case 00023:
					if((PSW_GET(PSW_N) ^ PSW_GET(PSW_V)) == 0) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 00024: /* BLT */
				case 00025:
				case 00026:
				case 00027:
					if(PSW_GET(PSW_N) ^ PSW_GET(PSW_V)) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 00030: /* BGT */
				case 00031:
				case 00032:
				case 00033:
					if((PSW_GET(PSW_Z) || (PSW_GET(PSW_N) ^ PSW_GET(PSW_V))) == 0) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 00034: /* BLE */
				case 00035:
				case 00036:
				case 00037:
					if(PSW_GET(PSW_Z) || (PSW_GET(PSW_N) ^ PSW_GET(PSW_V))) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
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
					dst = KD11CPUGetAddr(bus, insnjsr->rn, insnjsr->mode);
					src = r[insnjsr->r];
					CHECK();
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
					tmp = CPUREADNW(insn1->rn, insn1->mode);
					tmp = ~tmp;
					CPUWRITEW(insn1->rn, insn1->mode, tmp);
					PSW_CLR(PSW_V);
					PSW_SET(PSW_C);
					PSW_EQ(PSW_N, tmp & 0x8000);
					PSW_EQ(PSW_Z, !tmp);
					break;
				case 00052: /* INC */
					src = CPUREADNW(insn1->rn, insn1->mode);
					tmp = src + 1;
					CPUWRITEW(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_V, src == 077777)
					PSW_EQ(PSW_N, tmp & 0x8000);
					PSW_EQ(PSW_Z, !tmp);
					break;
				case 00053: /* DEC */
					src = CPUREADNW(insn1->rn, insn1->mode);
					tmp = src - 1;
					CPUWRITEW(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_V, src == 0100000)
					PSW_EQ(PSW_N, tmp & 0x8000);
					PSW_EQ(PSW_Z, !tmp);
					break;
				case 00054: /* NEG */
					tmp = CPUREADNW(insn1->rn, insn1->mode);
					if(tmp != 0100000) {
						tmp = -tmp;
					}
					CPUWRITEW(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_V, tmp == 0100000)
					PSW_EQ(PSW_N, tmp & 0x8000);
					PSW_EQ(PSW_Z, !tmp);
					PSW_EQ(PSW_C, tmp);
					break;
				case 00055: /* ADC */
					src = CPUREADNW(insn1->rn, insn1->mode);
					tmp2 = PSW_GET(PSW_C) ? 1 : 0;
					tmp = src + tmp2;
					CPUWRITEW(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_V, src == 0077777 && PSW_GET(PSW_C));
					PSW_EQ(PSW_C, src == 0177777 && PSW_GET(PSW_C));
					PSW_EQ(PSW_N, tmp & 0x8000);
					PSW_EQ(PSW_Z, !tmp);
					break;
				case 00056: /* SBC */
					src = CPUREADNW(insn1->rn, insn1->mode);
					tmp2 = PSW_GET(PSW_C) ? 1 : 0;
					tmp = src - tmp2;
					CPUWRITEW(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_V, src == 0100000);
					PSW_EQ(PSW_C, !src && PSW_GET(PSW_C));
					PSW_EQ(PSW_N, tmp & 0x8000);
					PSW_EQ(PSW_Z, !tmp);
					break;
				case 00057: /* TST */
					tmp = CPUREADW(insn1->rn, insn1->mode);
					PSW_CLR(PSW_V);
					PSW_CLR(PSW_C);
					PSW_EQ(PSW_N, tmp & 0x8000);
					PSW_EQ(PSW_Z, !tmp);
					break;
				case 00060: /* ROR */
					src = CPUREADNW(insn1->rn, insn1->mode);
					tmp2 = PSW_GET(PSW_C);
					tmp = src >> 1;
					if(tmp2) {
						tmp |= 0x8000;
					}
					CPUWRITEW(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_C, src & 0x0001);
					PSW_EQ(PSW_N, tmp & 0x8000);
					PSW_EQ(PSW_Z, !tmp);
					PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
					break;
				case 00061: /* ROL */
					src = CPUREADNW(insn1->rn, insn1->mode);
					tmp2 = PSW_GET(PSW_C);
					tmp = src << 1;
					if(tmp2) {
						tmp |= 0x0001;
					}
					CPUWRITEW(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_C, src & 0x8000);
					PSW_EQ(PSW_N, tmp & 0x8000);
					PSW_EQ(PSW_Z, !tmp);
					PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
					break;
				case 00062: /* ASR */
					src = CPUREADNW(insn1->rn, insn1->mode);
					tmp = src;
					if(tmp & 0x8000) {
						tmp >>= 1;
						tmp |= 0x8000;
					} else {
						tmp >>= 1;
					}
					CPUWRITEW(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_C, src & 1);
					PSW_EQ(PSW_N, tmp & 0x8000);
					PSW_EQ(PSW_Z, !tmp);
					PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
					break;
				case 00063: /* ASL */
					src = CPUREADNW(insn1->rn, insn1->mode);
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
					if(PSW_GET(PSW_N)) {
						tmp = 0xFFFF;
					} else {
						tmp = 0;
					}
					CPUWRITEW(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_Z, !PSW_GET(PSW_N));
					PSW_CLR(PSW_V);
					break;
				default: /* 006500-006677, 007000-007777: unused */
					TRCTrap(010, TRC_TRAP_ILL);
					TRAP(010);
					break;
			}
			break;
		case 001: /* MOV */
			tmp = CPUREADW(insn2->src_rn, insn2->src_mode);
			CPUWRITEW(insn2->dst_rn, insn2->dst_mode, tmp);
			PSW_EQ(PSW_N, tmp & 0x8000);
			PSW_EQ(PSW_Z, !tmp);
			PSW_CLR(PSW_V);
			break;
		case 002: /* CMP */
			src = CPUREADW(insn2->src_rn, insn2->src_mode);
			dst = CPUREADW(insn2->dst_rn, insn2->dst_mode);
			tmp = src - dst;
			PSW_EQ(PSW_N, tmp & 0x8000);
			PSW_EQ(PSW_Z, !tmp);
			PSW_EQ(PSW_V, ((src & 0x8000) != (dst & 0x8000)) \
					&& ((dst & 0x8000) == (tmp & 0x8000)));
			PSW_EQ(PSW_C, ((u32) src - (u32) dst) & 0x10000);
			break;
		case 003: /* BIT */
			src = CPUREADW(insn2->src_rn, insn2->src_mode);
			dst = CPUREADW(insn2->dst_rn, insn2->dst_mode);
			tmp = src & dst;
			PSW_EQ(PSW_N, tmp & 0x8000);
			PSW_EQ(PSW_Z, !tmp);
			PSW_CLR(PSW_V);
			break;
		case 004: /* BIC */
			src = CPUREADW(insn2->src_rn, insn2->src_mode);
			dst = CPUREADNW(insn2->dst_rn, insn2->dst_mode);
			tmp = ~src & dst;
			CPUWRITEW(insn2->dst_rn, insn2->dst_mode, tmp);
			PSW_EQ(PSW_N, tmp & 0x8000);
			PSW_EQ(PSW_Z, !tmp);
			PSW_CLR(PSW_V);
			break;
		case 005: /* BIS */
			src = CPUREADW(insn2->src_rn, insn2->src_mode);
			dst = CPUREADNW(insn2->dst_rn, insn2->dst_mode);
			tmp = src | dst;
			CPUWRITEW(insn2->dst_rn, insn2->dst_mode, tmp);
			PSW_EQ(PSW_N, tmp & 0x8000);
			PSW_EQ(PSW_Z, !tmp);
			PSW_CLR(PSW_V);
			break;
		case 006: /* ADD */
			src = CPUREADW(insn2->src_rn, insn2->src_mode);
			dst = CPUREADNW(insn2->dst_rn, insn2->dst_mode);
			tmp = src + dst;
			CPUWRITEW(insn2->dst_rn, insn2->dst_mode, tmp);
			PSW_EQ(PSW_N, tmp & 0x8000);
			PSW_EQ(PSW_Z, !tmp);
			PSW_EQ(PSW_V, ((src & 0x8000) == (dst & 0x8000)) \
					&& ((dst & 0x8000) != (tmp & 0x8000)));
			PSW_EQ(PSW_C, ((u32) src + (u32) dst) & 0x10000);
			break;
		case 007: /* 07 xx xx group */
			switch(insn >> 9) {
				case 0070: /* MUL */
					dst = r[insnjsr->r];
					src = CPUREADW(insnjsr->rn, insnjsr->mode);
					tmps32 = (s32) (s16) dst * (s16) src;
					r[insnjsr->r] = (u16) (tmps32 >> 16);
					r[insnjsr->r | 1] = (u16) tmps32;
					PSW_CLR(PSW_V);
					PSW_EQ(PSW_N, tmps32 < 0);
					PSW_EQ(PSW_Z, !tmps32);
					PSW_EQ(PSW_C, (tmps32 >= 0x7FFF) || (tmps32 < -0x8000));
					break;
				case 0071: /* DIV */
					tmps32 = (r[insnjsr->r] << 16)
						| r[insnjsr->r | 1];
					src = CPUREADW(insnjsr->rn, insnjsr->mode);
					if(src == 0) {
						PSW_SET(PSW_C);
						PSW_SET(PSW_V);
					} else {
						s32 quot = tmps32 / (s16) src;
						s32 rem = tmps32 % (s16) src;
						PSW_CLR(PSW_C);
						if((s16) quot != quot) {
							PSW_SET(PSW_V);
						} else {
							r[insnjsr->r] = (u16) quot;
							r[insnjsr->r | 1] = (u16) rem;
							PSW_EQ(PSW_Z, !quot);
							PSW_EQ(PSW_N, quot < 0);
						}
					}
					break;
				case 0072: /* ASH */
					dst = r[insnjsr->r];
					src = CPUREADW(insnjsr->rn, insnjsr->mode);
					if(src & 0x20) { /* negative; right */
						s16 stmp, stmp2;
						src = (~src & 0x1F) + 1;
						stmp = (s16) dst;
						stmp2 = stmp >> (src - 1);
						stmp >>= src;
						tmp = (u16) stmp;
						PSW_EQ(PSW_C, stmp2 & 1);
						PSW_CLR(PSW_V);
					} else if((src & 0x1F) == 0) {
						/* nothing */
						PSW_CLR(PSW_V);
						PSW_CLR(PSW_C);
						tmp = dst;
					} else { /* positive, left */
						s16 mask = 0;
						src &= 0x1F;
						tmp = dst << src;
						if(src > 0) {
							mask = 0x8000;
							mask >>= src;
							tmp2 = dst & mask;
							PSW_EQ(PSW_V, !((tmp2 == 0) || (((tmp2 & mask) | ~mask) == 0xFFFF)));
						} else {
							PSW_CLR(PSW_V);
						}
						PSW_EQ(PSW_C, (dst << (src - 1)) & 0x8000);
						if((dst & 0x8000) != (tmp & 0x8000)) {
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
					if((src & 0x3F) == 0x20) { /* negative; 32 right */
						PSW_EQ(PSW_C, tmps32 & 0x80000000);
						PSW_CLR(PSW_V);
						if(PSW_GET(PSW_C)) {
							tmps32 = 0xFFFFFFFF;
						} else {
							tmps32 = 0;
						}
					} else if(src & 0x20) { /* negative; right */
						s32 stmp2;
						src = (~src & 0x1F) + 1;
						stmp2 = tmps32 >> (src - 1);
						tmps32 >>= src;
						PSW_EQ(PSW_C, stmp2 & 1);
					} else if((src & 0x1F) == 0) {
						/* nothing */
						PSW_CLR(PSW_V);
						PSW_CLR(PSW_C);
					} else { /* positive, left */
						s32 stmp2;
						src &= 0x1F;
						stmp2 = tmps32 << (src - 1);
						tmps32 <<= src;
						PSW_EQ(PSW_C, stmp2 & 0x80000000);
						PSW_EQ(PSW_V, !!(dst & 0x8000)
								!= !!(tmps32 & 0x80000000));
					}
					r[insnjsr->r] = (u16) (tmps32 >> 16);
					r[insnjsr->r | 1] = (u16) tmps32;
					PSW_EQ(PSW_N, tmps32 & 0x80000000);
					PSW_EQ(PSW_Z, !tmps32);
					break;
				case 0074: /* XOR */
					src = r[insnjsr->r];
					dst = CPUREADNW(insnjsr->rn, insnjsr->mode);
					tmp = src ^ dst;
					CPUWRITEW(insnjsr->rn, insnjsr->mode, tmp);
					PSW_EQ(PSW_N, tmp & 0x8000);
					PSW_EQ(PSW_Z, !tmp);
					PSW_CLR(PSW_V);
					break;
				case 0075: /* FIS instructions */
					switch(insn >> 3) {
#ifdef USE_FLOAT
						case 007500: /* FADD */
							f1.u32 = (READ(r[insnrts->rn] + 4) << 16)
								| READ(r[insnrts->rn] + 6);
							f2.u32 = (READ(r[insnrts->rn]) << 16)
								| READ(r[insnrts->rn] + 2);
							f3.f32 = f1.f32 + f2.f32;
							/* TODO: result <= 2**-128 -> result = 0 */
							/* TODO: implement traps */
							WRITE(r[insnrts->rn] + 4,
									(u16) (f3.u32 >> 16));
							WRITE(r[insnrts->rn] + 6, (u16) f3.u32);
							r[insnrts->rn] += 4;
							PSW_EQ(PSW_N, f3.f32 < 0);
							PSW_EQ(PSW_Z, f3.f32 == 0);
							PSW_CLR(PSW_V);
							PSW_CLR(PSW_C);
							break;
						case 007501: /* FSUB */
							f1.u32 = (READ(r[insnrts->rn] + 4) << 16)
								| READ(r[insnrts->rn] + 6);
							f2.u32 = (READ(r[insnrts->rn]) << 16)
								| READ(r[insnrts->rn] + 2);
							f3.f32 = f1.f32 - f2.f32;
							/* TODO: result <= 2**-128 -> result = 0 */
							/* TODO: implement traps */
							WRITE(r[insnrts->rn] + 4,
									(u16) (f3.u32 >> 16));
							WRITE(r[insnrts->rn] + 6, (u16) f3.u32);
							r[insnrts->rn] += 4;
							PSW_EQ(PSW_N, f3.f32 < 0);
							PSW_EQ(PSW_Z, f3.f32 == 0);
							PSW_CLR(PSW_V);
							PSW_CLR(PSW_C);
							break;
						case 007502: /* FMUL */
							f1.u32 = (READ(r[insnrts->rn] + 4) << 16)
								| READ(r[insnrts->rn] + 6);
							f2.u32 = (READ(r[insnrts->rn]) << 16)
								| READ(r[insnrts->rn] + 2);
							f3.f32 = f1.f32 * f2.f32;
							/* TODO: result <= 2**-128 -> result = 0 */
							/* TODO: implement traps */
							WRITE(r[insnrts->rn] + 4,
									(u16) (f3.u32 >> 16));
							WRITE(r[insnrts->rn] + 6, (u16) f3.u32);
							r[insnrts->rn] += 4;
							PSW_EQ(PSW_N, f3.f32 < 0);
							PSW_EQ(PSW_Z, f3.f32 == 0);
							PSW_CLR(PSW_V);
							PSW_CLR(PSW_C);
							break;
						case 007503: /* FDIV */
							f1.u32 = (READ(r[insnrts->rn] + 4) << 16)
								| READ(r[insnrts->rn] + 6);
							f2.u32 = (READ(r[insnrts->rn]) << 16)
								| READ(r[insnrts->rn] + 2);
							if(f2.f32 != 0) {
								f3.f32 = f1.f32 / f2.f32;
								/* TODO: result <= 2**-128 -> result = 0 */
								/* TODO: implement traps */
								WRITE(r[insnrts->rn] + 4,
										(u16) (f3.u32 >> 16));
								WRITE(r[insnrts->rn] + 6,
										(u16) f3.u32);
								PSW_EQ(PSW_N, f3.f32 < 0);
								PSW_EQ(PSW_Z, f3.f32 == 0);
								PSW_CLR(PSW_V);
								PSW_CLR(PSW_C);
							}
							r[insnrts->rn] += 4;
							break;
#endif
						default:
							/* 075040-076777: unused */
							TRCTrap(010, TRC_TRAP_ILL);
							TRAP(010);
							break;
					}
					break;
				case 0077: /* SOB */
					r[insnsob->rn]--;
					if(r[insnsob->rn]) {
						r[7] -= 2 * insnsob->offset;
					}
					break;
				default:
					TRCTrap(010, TRC_TRAP_ILL);
					TRAP(010);
					break;
			}
			break;
		case 010: /* 10 xx xx group */
			switch(insn >> 6) {
				case 01000: /* BPL */
				case 01001:
				case 01002:
				case 01003:
					if(!PSW_GET(PSW_N)) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 01004: /* BMI */
				case 01005:
				case 01006:
				case 01007:
					if(PSW_GET(PSW_N)) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 01010: /* BHI */
				case 01011:
				case 01012:
				case 01013:
					if(!PSW_GET(PSW_C) && !PSW_GET(PSW_Z)) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 01014: /* BLOS */
				case 01015:
				case 01016:
				case 01017:
					if(PSW_GET(PSW_C) || PSW_GET(PSW_Z)) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 01020: /* BVC */
				case 01021:
				case 01022:
				case 01023:
					if(!PSW_GET(PSW_V)) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 01024: /* BVS */
				case 01025:
				case 01026:
				case 01027:
					if(PSW_GET(PSW_V)) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 01030: /* BCC */
				case 01031:
				case 01032:
				case 01033:
					if(!PSW_GET(PSW_C)) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 01034: /* BCS */
				case 01035:
				case 01036:
				case 01037:
					if(PSW_GET(PSW_C)) {
						r[7] += (s16) ((s8) insnbr->offset) * 2;
					}
					break;
				case 01040: /* EMT */
				case 01041:
				case 01042:
				case 01043:
					TRCTrap(030, TRC_TRAP);
					TRAP(030);
					break;
				case 01044: /* TRAP */
				case 01045:
				case 01046:
				case 01047:
					TRCTrap(034, TRC_TRAP);
					TRAP(034);
					break;
				case 01050: /* CLRB */
					CPUWRITEB(insn1->rn, insn1->mode, 0);
					PSW_CLR(PSW_N | PSW_V | PSW_C);
					PSW_SET(PSW_Z);
					break;
				case 01051: /* COMB */
					tmp = CPUREADNB(insn1->rn, insn1->mode);
					tmp = ~tmp;
					CPUWRITEB(insn1->rn, insn1->mode, tmp);
					PSW_CLR(PSW_V);
					PSW_SET(PSW_C);
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !((u8) tmp));
					break;
				case 01052: /* INCB */
					src = CPUREADNB(insn1->rn, insn1->mode);
					tmp = (u8) (src + 1);
					CPUWRITEB(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_V, src == 000177)
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !tmp);
					break;
				case 01053: /* DECB */
					src = CPUREADNB(insn1->rn, insn1->mode);
					tmp = (u8) (src - 1);
					CPUWRITEB(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_V, src == 0000200)
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !tmp);
					break;
				case 01054: /* NEGB */
					tmp = CPUREADNB(insn1->rn, insn1->mode);
					if(tmp != 0200) {
						tmp = -tmp;
					}
					CPUWRITEB(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_V, tmp == 0200)
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !tmp);
					PSW_EQ(PSW_C, tmp);
					break;
				case 01055: /* ADCB */
					src = CPUREADNB(insn1->rn, insn1->mode);
					tmp2 = PSW_GET(PSW_C) ? 1 : 0;
					tmp = (u8) (src + tmp2);
					CPUWRITEB(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_V, src == 0177 && PSW_GET(PSW_C));
					PSW_EQ(PSW_C, src == 0377 && PSW_GET(PSW_C));
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !tmp);
					break;
				case 01056: /* SBCB */
					src = CPUREADNB(insn1->rn, insn1->mode);
					tmp2 = PSW_GET(PSW_C) ? 1 : 0;
					tmp = (u8) (src - tmp2);
					CPUWRITEB(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_V, src == 0200);
					PSW_EQ(PSW_C, !src && PSW_GET(PSW_C));
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !tmp);
					break;
				case 01057: /* TSTB */
					tmp = CPUREADB(insn1->rn, insn1->mode);
					PSW_CLR(PSW_V);
					PSW_CLR(PSW_C);
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !tmp);
					break;
				case 01060: /* RORB */
					src = CPUREADNB(insn1->rn, insn1->mode);
					tmp2 = PSW_GET(PSW_C);
					tmp = src >> 1;
					if(tmp2) {
						tmp |= 0x80;
					}
					CPUWRITEB(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_C, src & 0x01);
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !tmp);
					PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
					break;
				case 01061: /* ROLB */
					src = CPUREADNB(insn1->rn, insn1->mode);
					tmp2 = PSW_GET(PSW_C);
					tmp = (u8) (src << 1);
					if(tmp2) {
						tmp |= 0x01;
					}
					CPUWRITEB(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_C, src & 0x80);
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !tmp);
					PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
					break;
				case 01062: /* ASRB */
					src = CPUREADNB(insn1->rn, insn1->mode);
					tmp = src;
					if(tmp & 0x80) {
						tmp >>= 1;
						tmp |= 0x80;
					} else {
						tmp >>= 1;
					}
					CPUWRITEB(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_C, src & 1);
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !tmp);
					PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
					break;
				case 01063: /* ASLB */
					src = CPUREADNB(insn1->rn, insn1->mode);
					tmp = (u8) (src << 1);
					CPUWRITEB(insn1->rn, insn1->mode, tmp);
					PSW_EQ(PSW_C, src & 0x80);
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !tmp);
					PSW_EQ(PSW_V, PSW_GET(PSW_N) ^ PSW_GET(PSW_C));
					break;
				case 01064: /* MTPS */
					tmp = CPUREADB(insn1->rn, insn1->mode);
					psw = (psw & PSW_T) | (tmp & ~PSW_T);
					break;
				case 01067: /* MFPS */
					tmp = (u8) psw;
					if(insn1->mode == 0) {
						r[insn1->rn] = (s8) psw;
					} else {
						CPUWRITEB(insn1->rn, insn1->mode, tmp);
					}
					PSW_EQ(PSW_N, tmp & 0x80);
					PSW_EQ(PSW_Z, !(tmp & 0xFF));
					PSW_CLR(PSW_V);
					break;
				default:
					/* unused */
					TRCTrap(010, TRC_TRAP_ILL);
					TRAP(010);
					break;
			}
			break;
		case 011: /* MOVB */
			tmp = CPUREADB(insn2->src_rn, insn2->src_mode);
			tmp = (s8) tmp;
			if(insn2->dst_mode == 0) {
				r[insn2->dst_rn] = tmp;
			} else {
				CPUWRITEB(insn2->dst_rn, insn2->dst_mode, tmp);
			}
			PSW_EQ(PSW_N, tmp & 0x80);
			PSW_EQ(PSW_Z, !tmp);
			PSW_CLR(PSW_V);
			break;
		case 012: /* CMPB */
			src = CPUREADB(insn2->src_rn, insn2->src_mode);
			dst = CPUREADB(insn2->dst_rn, insn2->dst_mode);
			tmp = (u8) (src - dst);
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
			tmp = (u8) (~src & dst);
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
			src = CPUREADW(insn2->src_rn, insn2->src_mode);
			dst = CPUREADNW(insn2->dst_rn, insn2->dst_mode);
			tmp = dst - src;
			CPUWRITEW(insn2->dst_rn, insn2->dst_mode, tmp);
			PSW_EQ(PSW_N, tmp & 0x8000);
			PSW_EQ(PSW_Z, !tmp);
			PSW_EQ(PSW_V, ((src & 0x8000) != (dst & 0x8000)) \
					&& ((src & 0x8000) == (tmp & 0x8000)));
			PSW_EQ(PSW_C, ((u32) dst - (u32) src) & 0x10000);
			break;
		default: /* unused */
			TRCTrap(010, TRC_TRAP_ILL);
			TRAP(010);
			break;
	}
}


// ToDo: Resolve trap confusion
void KD11::KD11HandleTraps(QBUS* bus)
{
	u16 trap = this->trap;

	if(!PSW_GET(PSW_PRIO)) {
		this->trap = bus->trap;
		bus->trap = 0;
	} else if(bus->trap == 004) {
		this->trap = bus->trap;
		bus->trap = 0;
	} else {
		this->trap = 0;
	}

	/* ignore traps if in HALT mode */
	if(runState == STATE_HALT) {
		return;
	}

	if(!trap) {
		if(this->trap) {
			trap = this->trap;
			this->trap = 0;
		} else {
			return;
		}
	}

	/* trap instructions have highest priority */
	if((this->trap == 030 || this->trap == 034)
			&& !(trap == 030 || trap == 034)) {
		u16 tmp = this->trap;
		this->trap = trap;
		trap = tmp;
	}

	TRCCPUEvent(TRC_CPU_TRAP, trap);

	r[6] -= 2;
	WRITE(r[6], psw);
	if(bus->trap == 004 || bus->trap == 010) {
		TRCCPUEvent(TRC_CPU_DBLBUS, r[6]);
		bus->trap = 0;
		runState = STATE_HALT;
		return;
	}

	r[6] -= 2;
	WRITE(r[6], r[7]);
	if(bus->trap == 004 || bus->trap == 010) {
		TRCCPUEvent(TRC_CPU_DBLBUS, r[6]);
		bus->trap = 0;
		runState = STATE_HALT;
		return;
	}

	r[7] = READ(trap);
	if(bus->trap == 004 || bus->trap == 010) {
		TRCCPUEvent(TRC_CPU_DBLBUS, trap);
		bus->trap = 0;
		runState = STATE_HALT;
		return;
	}

	psw = READ(trap + 2);
	if(bus->trap == 004 || bus->trap == 010) {
		TRCCPUEvent(TRC_CPU_DBLBUS, trap + 2);
		bus->trap = 0;
		runState = STATE_HALT;
		return;
	}

	/* resume execution if in WAIT state */
	if(runState == STATE_WAIT) {
		TRCCPUEvent(TRC_CPU_RUN, r[7]);
		runState = STATE_RUN;
	}
}

void KD11::step(QBUS* bus)
{
	switch(runState) {
		case STATE_HALT:
			KD11ODTStep(bus);
			break;
		case STATE_RUN:
			IFTRC() {
				TRCSETIGNBUS();
				u16 bus_trap = bus->trap;
				u16 cpu_trap = trap;
				u16 code[3];
				code[0] = READ(r[7] + 0);
				code[1] = READ(r[7] + 2);
				code[2] = READ(r[7] + 4);
				TRCStep(r, psw, code);
				bus->trap = bus_trap;
				trap = cpu_trap;
				TRCCLRIGNBUS();
			}
			KD11CPUStep(bus);
			if(runState == STATE_INHIBIT_TRACE) {
				runState = STATE_RUN;
			} else if(!trap && (psw & PSW_T)) {
				TRCTrap(014, TRC_TRAP_T);
				TRAP(014);
			}
			KD11HandleTraps(bus);
			if(runState == STATE_HALT) {
				odt.state = ODT_STATE_INIT;
			}
			break;
		case STATE_WAIT:
			KD11HandleTraps(bus);
			break;
	}
}

void KD11::setTrap(int n)
{
	if(trap == 0 || trap > n)
		trap = n;
}

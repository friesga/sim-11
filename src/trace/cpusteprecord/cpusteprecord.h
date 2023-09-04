#ifndef _CPUSTEPRECORD_H_
#define _CPUSTEPRECORD_H_

#include "types.h"
#include "../recordheader.h"

#include <algorithm>
#include <iterator>
#include <iomanip>

using std::oct;
using std::setw;
using std::setfill;

// The following macro's are defined in types.h and kd11_na_cpu.h and are to be
// converted to functions.
#define	_BV(x)	(1 << (x))

#define	PSW_C			_BV(0)
#define	PSW_V			_BV(1)
#define	PSW_Z			_BV(2)
#define	PSW_N			_BV(3)
#define	PSW_T			_BV(4)
#define	PSW_PRIO		_BV(7)
#define	PSW_PRIORITY	(_BV(7) | _BV(6) | _BV(5))

#define	PSW_BIT(x)		(((record.psw_) & (x)) ? 1 : 0)

// Definition of the type to discriminate the trace records in the template
// classes.
struct CpuStepRecord {};

// Specialization of the TraceRecord for the TraceCpu record
template <>
class TraceRecord<CpuStepRecord>
{
	friend std::ostream& operator<< (std::ostream& os, 
		TraceRecord<CpuStepRecord> record);

	// Define constants to be used in this class as enums as that's the only
	// C++ way to define class scope (non-static) constants.
	enum { numRegisters = 8};
	enum { maxInstructionSize = 3};

	u16 r_[numRegisters];
	u16	psw_;
	u16	insn_[maxInstructionSize];

	char priorityBit () { return (psw_ & PSW_PRIO) ? 'P' : '-'; }
	char bit6 () { return (psw_ & _BV(6)) ? 'p' : '-'; }
	char bit5 () { return (psw_ & _BV(5)) ? 'p' : '-'; }
	char traceBit ()    { return (psw_ & PSW_T) ? 'T' : '-'; }
	char negativeBit () { return (psw_ & PSW_N) ? 'N' : '-'; }
	char zeroBit ()     { return (psw_ & PSW_Z) ? 'Z' : '-'; }
	char overflowBit () { return (psw_ & PSW_V) ? 'V' : '-'; }
	char carryBit ()    { return (psw_ & PSW_C) ? 'C' : '-'; }

	int LSI11Disassemble (const u16* insn, u16 pc, char* buf);
    int LSI11Write (char* buf, const char* str, int pos);
    int LSI11WriteN (char* buf, u16 val, int pos);
    const u16* LSI11DisassemblePCOperand (u8 rn, u8 mode, const u16* x,
        u16* pc, char* buf, int* p);
    const u16* LSI11DisassembleOperand(u8 rn, u8 mode, const u16* x,
        u16* pc, char* buf, int* p);
    int LSI11DisassembleBranch(s8 offset, u16 pc, char* buf, int pos);
    int LSI11OperandLength(const u8 rn, const u8 mode);
    int LSI11InstructionLength(const u16* insn);


public:
	TraceRecord ();
	TraceRecord (u16 r[], u16 psw, u16 insn[]);

	Magic magic () {return Magic::CPU0;}
};

// Default constructor for the TraceCpu record
inline TraceRecord<CpuStepRecord>::TraceRecord ()
	:
	r_ {},
	psw_ {0},
	insn_ {0}
{}

inline TraceRecord<CpuStepRecord>::TraceRecord (u16 r[], u16 psw, u16 insn[])
	:
	psw_ {psw}
{
	std::copy (r, r + numRegisters, r_);
	std::copy (insn, insn + maxInstructionSize, insn_);
}


#endif // _CPUSTEPRECORD_H_
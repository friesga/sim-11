#ifndef _CPUDATA_H_
#define _CPUDATA_H_

#include "types.h"
#include "conddata/conddata.h"
#include "interruptrequest/interruptrequest.h"
#include "kd/include/generalregisters.h"
#include "kd/include/psw.h"
#include "kd/common/conditioncodes/conditioncodes.h"
#include "trace/trace.h"

// This interface provides access to the CPU data the instruction classes
// need to be able to decode the operands adresses and execute the function.
// To this end the interface has to give access to its execution state and
// provide access to the memory via the CPU.
//
// For a LSI-11 the execution state is defined by the eight general registers
// and the processor status word.
// 
// The CpuData interface also provides access to the CpuControl functions, to
// allow the Operate Group instructions, such as HALT and RESET, to access the
// control functions.
//
class CpuData
{
public:
	// Definition of the trap conditions. The vectors corresponding to these
	// conditions are defined in a map.
	enum class TrapCondition
	{
		None,						// Reserved
		BusError,					// Time out and other errors
		ReservedInstructionTrap,	// Reserved instruction
		IllegalInstructionTrap,		// Illegal and reserved instructions
		BreakpointTrap,				// BPT instruction
		InputOutputTrap,			// IOT instruction
		PowerFail,					// Power fail
		EmulatorTrap,				// EMT instruction
		TrapInstruction,			// TRAP instruction
		FIS,						// Floating point
		StackOverflow,				// Stack overflow
		MemoryManagementTrap		// Memory management abort
	};

	// Definition of the KD11 execution state
    virtual GeneralRegisters& registers () = 0;
    virtual PSW& psw () = 0;
	virtual void loadPSW (u16 value) = 0;
	virtual void setCC (ConditionCodes conditionCodes) = 0;
	virtual bool stackOverflow () = 0;

	// The memory access functions must be able to generate a bus error trap
	// when accessing non-existing memory.
	virtual TrapCondition trap () = 0;
    virtual void setTrap (TrapCondition trap, TrapRecordType cause = TrapRecordType::TRAP) = 0;
	virtual void clearTrap () = 0;
	virtual u16 trapVector () = 0;
	virtual u16 trapVector (TrapCondition trap) = 0;
};

#endif // _CPUDATA_H_
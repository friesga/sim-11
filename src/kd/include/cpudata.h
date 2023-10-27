#ifndef _CPUDATA_H_
#define _CPUDATA_H_

#include "types.h"
#include "cpucontrol.h"
#include "cpuexecution.h"
#include "conddata/conddata.h"
#include "interruptrequest/interruptrequest.h"
#include "kd/include/generalregisters.h"
#include "kd/common/conditioncodes/conditioncodes.h"

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
class CpuData : public CpuControl, public CpuExecution
{
public:
	// Definition of the trap conditions and the vector belonging to the
	// condition.
	// 
	// The Trap enum is defined as a non-class enum to be able to use it
	// as vector addresses.
	enum Trap
	{
		None = 0,						// Reserved
		BusError = 004,					// Time out and other errors
		IllegalInstructionTrap = 010,	// Illegal and reserved instructions
		BreakpointTrap = 014,			// BPT instruction
		InputOutputTrap = 020,			// IOT instruction
		PowerFail = 024,				// Power fail
		EmulatorTrap = 030,				// EMT instruction
		TrapInstruction = 034,			// TRAP instruction
		FIS = 0244						// Floating point
	};

	// Definition of the KD11 execution state
    virtual GeneralRegisters& registers () = 0;
    virtual u16& psw () = 0;
	virtual void setCC (ConditionCodes conditionCodes) = 0;

	// Definition of functions to access memory
	virtual CondData<u16> fetchWord (u16 address) = 0;
	virtual CondData<u8> fetchByte (u16 address) = 0;
	virtual bool putWord (u16 address, u16 value) = 0;
	virtual bool putByte (u16 address, u8 value) = 0;
	virtual bool pushWord (u16 value) = 0;
	virtual bool popWord (u16 *destination) = 0;

	// The memory access functions must be able to generate a bus error trap
	// whem accessing non-existing memory.
	// 
    virtual void setTrap (Trap trap) = 0;
};

#endif // _CPUDATA_H_
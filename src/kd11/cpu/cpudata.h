#ifndef _CPUDATA_H_
#define _CPUDATA_H_

#include "types.h"
#include "cpucontrol.h"
#include "conddata/conddata.h"
#include "interruptrequest/interruptrequest.h"

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
class CpuData : public CpuControl
{
public:
	// The Trap enum is defined as a non-class enum to be able to use it
	// as an array index.
	enum Trap
	{
		None,
		BusError,
		IllegalInstructionTrap,
		BreakpointTrap,
		InputOutputTrap,
		EmulatorTrap,
		TrapInstruction,
		FIS
	};

	using GeneralRegisters = u16[8];

	// Definition of the LSI-11/2 execution state
    virtual GeneralRegisters& registers () = 0;
    virtual u16& psw () = 0;

	// Definition of functions to access memory
	virtual CondData<u16> fetchWord (u16 address) = 0;
	virtual CondData<u8> fetchByte (u16 address) = 0;
	virtual bool putWord (u16 address, u16 value) = 0;
	virtual bool putByte (u16 address, u8 value) = 0;
	virtual void pushWord (u16 value) = 0;
	virtual bool popWord (u16 *destination) = 0;
};

#endif // _CPUDATA_H_
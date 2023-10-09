#ifndef _CPUCONTROL_H_
#define _CPUCONTROL_H_

#include "interruptrequest/interruptrequest.h"
#include "types.h"

// This interface defines the function a cpu has to implement to be able to
// control it from the console.
class CpuControl
{
public:
    // This enum holds the reason for halting the system. This data is printed
	// by the ODT Maintenance command.
	// (LSI11 PDP11/03 Processor Handbook)
	enum class HaltReason
	{
		HaltInstruction			= 0,	// Halt instruction or B Halt line
		BusErrorOnIntrptVector  = 1,	// Bus Error occurred while getting device interrupt vector
		BusErrorOnMemoryRefresh = 2,	// Not used
		DoubleBusError			= 3,	// Double Bus Error occurred (stack was non-existent value)
		NonExistentMicroAddress = 4		// Not used
	};

    // The setTrap function is needed for the execution of a reset in power-up
    // mode 0 (start via vector at address 24/26).
    virtual void setTrap (InterruptRequest const *ir) = 0;
    virtual void loadTrapVector (InterruptRequest const* trap) = 0;
    virtual void cpuReset () = 0;
    virtual void busReset () = 0;
    virtual void halt () = 0;
    virtual void wait () = 0;
    virtual void start (u16 address) = 0;
    virtual void proceed () = 0;
    virtual void inhibitTraceTrap () = 0;

    virtual u16 registerValue (u8 registerNr) = 0;
    virtual void setRegister (u8 registerNr, u16 value) = 0;

    virtual u16 pswValue () = 0;
    virtual void setPSW (u16 value) = 0;

    virtual HaltReason haltReason () = 0;
};


#endif // _CPUCONTROL_H_
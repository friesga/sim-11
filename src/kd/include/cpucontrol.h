#ifndef _CPUCONTROL_H_
#define _CPUCONTROL_H_

#include "interruptrequest/interruptrequest.h"
#include "types.h"

// This interface defines the function a cpu has to implement to be able to
// control it from the console.
class CpuControl
{
public:
    // Definition of CPU run states
    enum class CpuRunState
    {
        HALT,
        RUN,
        WAIT
    };

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

    virtual void cpuReset () = 0;
    virtual void busReset () = 0;
    virtual void halt () = 0;

    // The HaltMode is used by the KDF11-U. The ODT in this CPU entails a 
    // "Toggle Halt" command which toggles a halt flip-flop located in the CPU.
    // CPU's not supporting this command should implement a dummy version of
    // setHaltMode and return false on inHaltMode().
    virtual void setHaltMode (bool haltMode) = 0;
    virtual bool inHaltMode () = 0;

    virtual void wait () = 0;
    virtual void start (u16 address) = 0;
    virtual void proceed () = 0;
    virtual HaltReason haltReason () = 0;

    // The step has to return true if a following instruction can be execued,
    // false otherwise. In the latter case a HALT instruction was executed.
    virtual bool step () = 0;
};


#endif // _CPUCONTROL_H_
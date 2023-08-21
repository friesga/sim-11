#ifndef _CPUCONTROL_H_
#define _CPUCONTROL_H_

#include "interruptrequest/interruptrequest.h"
#include "types.h"

// This interface defines the function a cpu has to implement to be able to
// control it from the console.
class CpuControl
{
public:
    // The setTrap function is needed for the execution of a reset in power-up
    // mode 0 (start via vector at address 24/26).
    virtual void setTrap (InterruptRequest const *ir) = 0;
    virtual void reset () = 0;
    virtual void halt () = 0;
    virtual void wait () = 0;
    virtual void start (u16 address) = 0;
    virtual void proceed () = 0;
    virtual void inhibitTraceTrap () = 0;

    virtual u16 registerValue (u8 registerNr) = 0;
    virtual void setRegister (u8 registerNr, u16 value) = 0;

    virtual u16 pswValue () = 0;
    virtual void setPSW (u16 value) = 0;
};


#endif // _CPUCONTROL_H_
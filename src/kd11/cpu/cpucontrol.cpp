#include "kd11cpu.h"
#include "cpucontrol.h"

// Halt the processor
void KD11CPU::halt ()
{
    runState = STATE_HALT;
    bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::False);
}

// Start the processor at the given address
void KD11CPU::start (u16 address)
{
    register_[7] = address;
    runState = STATE_RUN;
    bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::True);
}

// Continue execution at the current PC
void KD11CPU::proceed ()
{
    runState = STATE_RUN;
    bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::True);
}

// Return the value of a register. Access to the registers and PSW has to be
// provided via special functions as the KD11 has no registers to access them.
u16 KD11CPU::registerValue (u8 registerNr)
{
    return register_[registerNr];
}

// Set the given register to the given value
void KD11CPU::setRegister (u8 registerNr, u16 value)
{
    register_[registerNr] = value;
}

// Return the value of the Processor Status Word
 u16 KD11CPU::pswValue ()
 {
     return psw;
 }

 // Set the Processor Status Word to the given value. The T-bit cannot be set
 // via this function.
 void KD11CPU::setPSW (u16 value)
 {
     psw = (psw & PSW_T) | (value & ~PSW_T);
 }

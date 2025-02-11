#ifndef _DISPLAYREGISTER_H_
#define _DISPLAYREGISTER_H_

#include "abstractbusdevice/abstractbusdevice.h"
#include "statuscodes.h"

// The display register is a write-only register located at address
// 017777570. Any attempt to read from this register will result in a trap
// through the vector at location 4. This means that instructions which
// perform a DATIP/DATO sequence (INC, DEC, and CLR) may not be used to
// access the display register. When accessing the display register, a MOV
// instruction is the preferred instruction.This type of access is in keeping
// with the operation of the PDP-11/70. This register uses only bits 1 and 0
// which, when set, light two LEDs located on the CPU module.The display
// register is used by the power-up diagnostics as they run.Three different
// sections of the PDP-11/24 are tested by these diagnostics, and the display
// register LEDs are used to indicate the condition of each test.
// Source: PDP-11/24 Technical Manual (EK-11024-TM-003)
//
// This register is not part of the KDF11-A and KDF11-B processors and 
// therefore is implemented as a separate CPU Module.
//
class DisplayRegister : public AbstractBusDevice
{
public:
	// Functions required for the BusDevice interface.
	CondData<u16> read (BusAddress address);
	StatusCode writeWord (BusAddress address, u16 value);
	bool responsible (BusAddress address);
	void reset ();

private:
	static const u16 displayRegister {0177570};
	u16 displayRegister_ {0};
};

#endif // _DISPLAYREGISTER_H_
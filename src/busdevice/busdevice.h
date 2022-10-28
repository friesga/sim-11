#ifndef _BUSDEVICE_H_
#define _BUSDEVICE_H_

#include "types.h"
#include "statuscodes.h"

#include <string>
#include <memory>

class QBUS;
class Unit;

// Definition of the functions every QBUS module should provide.
class BusDevice
{
protected:
    std::string name_;          // Device name
    u32 baseAddress_;           // Device registers base address
    u32 vector_;                // Device vector

public:
	// ToDo: Make QBUS* private
	QBUS*	bus;

	// Required functions
	virtual StatusCode read (u16 addr, u16 *destination) = 0;
	virtual StatusCode writeWord (u16 addr, u16 value) = 0;
	virtual StatusCode writeByte (u16 addr, u8 value);
	virtual bool responsible (u16 addr) = 0;
	virtual void reset () = 0;

	// Optional functions
	std::string name() { return name_; }
};

#endif // _BUSDEVICE_H_
#ifndef _QBUSMODULE_H_
#define _QBUSMODULE_H_

#include "types.h"
# include "statuscodes.h"

class QBUS;

// Define the functions every QBUS module should provide
class QBUSModule
{
public:
	QBUS*	bus;
	virtual StatusCode read (u16 addr, u16 *destination) = 0;
	virtual StatusCode writeWord (u16 addr, u16 value) = 0;
	virtual StatusCode writeByte (u16 addr, u8 value);
	// virtual void service (Unit &unit) = 0;
	virtual u8 responsible (u16 addr) = 0;
	virtual void reset () = 0;
};

#endif // _QBUSMODULE_H_
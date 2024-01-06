#ifndef _PDP11PERIPHERAL_H_
#define _PDP11PERIPHERAL_H_

#include "types.h"
#include "statuscodes.h"
#include "abstractbusdevice/abstractbusdevice.h"

#include <string>
#include <memory>

class Qbus;
class Unit;

// Definition of some data members and functions common to all PDP-11
// peripheral classes.
class PDP11Peripheral : public AbstractBusDevice
{
protected:
	Qbus* bus_;
    std::string name_;          // Device name
    u16 baseAddress_;           // Device registers base address
    u16 vector_;                // Device vector

public:
	PDP11Peripheral (Qbus *bus);
	virtual ~PDP11Peripheral () {};
	
	std::string name() { return name_; }
};

#endif // _PDP11PERIPHERAL_H_
#ifndef _ABSTRACTBUSDEVICE_H_
#define _ABSTRACTBUSDEVICE_H_

#include "busdevice.h"
#include "statuscodes.h"

// This class acts as a common base class for the peripherals and
// the PDP11Processor classes. The class implements the writeByte() function
// as this function is common for peripherals and PDP11Processor.
//
class AbstractBusDevice : public BusDevice
{
public:
	StatusCode writeByte (BusAddress address, u8 value) override;
};

#endif // !_ABSTRACTBUSDEVICE_H_

#ifndef _ABSTRACTBUSDEVICE_H_
#define _ABSTRACTBUSDEVICE_H_

#include "busdevice/busdevice.h"
#include "statuscodes.h"

// This class acts as a common base class for the PDP11Peripheral and
// PDP11Processor classes. The class implements the writeByte() function
// as this function is common for PDP11Peripheral and PDP11Processor.
//
class AbstractBusDevice : public BusDevice
{
public:
	StatusCode writeByte (u16 addr, u8 value) override;
};

#endif // !_ABSTRACTBUSDEVICE_H_

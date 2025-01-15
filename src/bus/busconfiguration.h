#ifndef _BUSCONFIGURATION_H_
#define _BUSCONFIGURATION_H_

#include "busdevice.h"
#include "unibusmap.h"

// This interface declares the functions required to configure a bus
class BusConfiguration
{
public:
	
	virtual bool installModuleAtPosition (BusDevice* module, size_t position) = 0;
	virtual bool installModule (BusDevice* module) = 0;
	virtual void installUnibusMap (UnibusMap* device) = 0;
};

#endif // _BUSCONFIGURATION_H_
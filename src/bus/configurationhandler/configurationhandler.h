#ifndef _CONFIGURATIONHANDLER_H_
#define _CONFIGURATIONHANDLER_H_

#include "bus/include/busconfiguration.h"

class ConfigurationHandler : public BusConfiguration
{
public:
	bool installModuleAtPosition (BusDevice* module, size_t position);
	bool installModule (BusDevice* module);
	void installUnibusMap (UnibusMap* device);
	BusDevice* responsibleModule (BusAddress address);

	Iterator begin ();
	Iterator end ();
	Iterator operator[] (int index);

	void reset ();

private:
	// A BA11-N backplane has nine slots, named ROW 1 to ROW 9. ROW 1 corresponds
// with slot[0], ROW 9 with slot [8].
	size_t numDevices_ {0};
	static const size_t numberOfSlots {9};
	BusDevice* slots_[numberOfSlots] {nullptr};

	UnibusMap* unibusMap_ {nullptr};
};

#endif // _CONFIGURATIONHANDLER_H_
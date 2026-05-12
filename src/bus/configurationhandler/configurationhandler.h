#ifndef _CONFIGURATIONHANDLER_H_
#define _CONFIGURATIONHANDLER_H_

#include "bus/include/busconfiguration.h"

class ConfigurationHandler : public BusConfiguration
{
public:
	bool installModuleAtPosition (BusDevice* module, size_t position) override;
	bool installModule (BusDevice* module) override;
	BusDevice* responsibleModule (BusAddress address) override;

	Iterator begin () override;
	Iterator end () override;
	size_t capacity () override;
	Iterator operator[] (int index) override;

	void reset () override;

private:
	// A BA11-N backplane has nine slots, named ROW 1 to ROW 9. ROW 1 corresponds
	// with slot[0], ROW 9 with slot [8].
	static const size_t numberOfSlots_ {32};
	size_t numDevices_ {0};

	BusDevice* slots_[numberOfSlots_] {nullptr};

	UnibusMap* unibusMap_ {nullptr};
};

#endif // _CONFIGURATIONHANDLER_H_
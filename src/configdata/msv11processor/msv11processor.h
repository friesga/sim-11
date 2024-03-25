#ifndef _MSV11PROCESSOR_H_
#define _MSV11PROCESSOR_H_

#include "../sectionprocessor/deviceconfigprocessor.h"
#include "../msv11config/msv11config.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::string;

class MSV11Processor : public DeviceConfigProcessor
{
	unique_ptr<MSV11Config> msv11ConfigPtr {nullptr};

    // Define process as a pointer to a BDV11Processor member function
	// with a iniparser::Value argument and returning void.

	typedef void (MSV11Processor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"power_source", &MSV11Processor::processPowerSource},
		{"starting_address", &MSV11Processor::processStartingAddress},
		{"bank7_lower2K_enabled", &MSV11Processor::processBank7Lower2kW}
	};

	map<string, MSV11Config::PowerSource> validPowerSources =
	{
		{"system",         MSV11Config::PowerSource::System},
		{"battery_backup", MSV11Config::PowerSource::BatteryBackup},
	};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);
	void processPowerSource (iniparser::Value value);
	void processStartingAddress (iniparser::Value value);
	void processBank7Lower2kW (iniparser::Value value);

public:
	MSV11Processor ();
	DeviceConfig getConfig ();
};


#endif // _MSV11PROCESSOR_H_
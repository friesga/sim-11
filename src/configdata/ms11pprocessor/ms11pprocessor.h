#ifndef _MS11PPROCESSOR_H_
#define _MS11PPROCESSOR_H_

#include "../sectionprocessor/deviceconfigprocessor.h"
#include "../ms11pconfig/ms11pconfig.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::string;

class MS11PProcessor : public DeviceConfigProcessor
{
	unique_ptr<MS11PConfig> ms11pConfigPtr {nullptr};

	typedef void (MS11PProcessor::* Process)(iniparser::Value);

	map<string, Process> valueProcessors =
	{
		{"power_source", &MS11PProcessor::processPowerSource},
		{"starting_address", &MS11PProcessor::processStartingAddress},
		{"csr_address", &MS11PProcessor::processCsrAddress}
	};

	map<string, MS11PConfig::PowerSource> validPowerSources =
	{
		{"system",         MS11PConfig::PowerSource::System},
		{"battery_backup", MS11PConfig::PowerSource::BatteryBackup},
	};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section* subSection);
	void processPowerSource (iniparser::Value value);
	void processStartingAddress (iniparser::Value value);
	void processCsrAddress (iniparser::Value value);

public:
	MS11PProcessor ();
	DeviceConfig getConfig ();
};


#endif // _MS11PPROCESSOR_H_
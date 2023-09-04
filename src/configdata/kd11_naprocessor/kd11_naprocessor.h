#ifndef _KD11_NAPROCESSOR_H_
#define _KD11_NAPROCESSOR_H_

#include "../sectionprocessor/sectionprocessor.h"
#include "../kd11_naconfig/kd11_naconfig.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::map;
using std::string;

class KD11_NAProcessor : public SectionProcessor
{
	unique_ptr<KD11_NAConfig> kd11ConfigPtr {nullptr};

	// Define process as a pointer to a KD11Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (KD11_NAProcessor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"power-up_mode", &KD11_NAProcessor::processPowerUpMode}
	};

	map<string, KD11_NAConfig::PowerUpMode> validPowerUpModes =
	{
		{"vector",    KD11_NAConfig::PowerUpMode::Vector},
		{"ODT",       KD11_NAConfig::PowerUpMode::ODT},
		{"bootstrap", KD11_NAConfig::PowerUpMode::Bootstrap},
	};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void processPowerUpMode (iniparser::Value value);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);


public:
	KD11_NAProcessor ();
	unique_ptr<DeviceConfig> getConfig () override;
};

#endif // _KD11_NAPROCESSOR_H_
#ifndef _KD11PROCESSOR_H_
#define _KD11PROCESSOR_H_

#include "../sectionprocessor/sectionprocessor.h"
#include "../kd11config/kd11config.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::map;
using std::string;

class KD11Processor : public SectionProcessor
{
	unique_ptr<KD11Config> kd11ConfigPtr {nullptr};

	// Define process as a pointer to a KD11Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (KD11Processor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"power-up_mode", &KD11Processor::processPowerUpMode}
	};

	map<string, KD11Config::PowerUpMode> validPowerUpModes =
	{
		{"vector",    KD11Config::PowerUpMode::Vector},
		{"ODT",       KD11Config::PowerUpMode::ODT},
		{"bootstrap", KD11Config::PowerUpMode::Bootstrap},
	};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void processPowerUpMode (iniparser::Value value);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);


public:
	KD11Processor ();
	unique_ptr<DeviceConfig> getConfig () override;
};

#endif // _KD11PROCESSOR_H_
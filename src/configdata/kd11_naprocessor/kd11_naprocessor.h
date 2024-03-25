#ifndef _KD11_NAPROCESSOR_H_
#define _KD11_NAPROCESSOR_H_

#include "configdata/sectionprocessor/sectionprocessor.h"
#include "../kd11processor/kd11processor.h"
#include "../kd11_naconfig/kd11_naconfig.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

class KD11_NAProcessor : public SectionProcessor
{
public:
	KD11_NAProcessor ();
	DeviceConfig getConfig () override;

private:
	// Define process as a pointer to a KD11Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (KD11_NAProcessor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"power-up_mode", &KD11_NAProcessor::processPowerUpMode}
	};

	void processValue (iniparser::Section::ValueIterator valueIterator) override;
	void checkConsistency () override;
	void processSubsection (iniparser::Section *subSection) override;
	void processPowerUpMode (iniparser::Value value);

	unique_ptr<KD11_NAConfig> kd11_naConfigPtr {make_unique<KD11_NAConfig> ()};
};

#endif // _KD11_NAPROCESSOR_H_
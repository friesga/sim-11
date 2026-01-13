#ifndef _M792PROCESSOR_H_
#define _M792PROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "configdata/m792/m792config/m792config.h"

#include <map>

using std::map;

// The processor checks the M792 section has an option key and that this
// option equals to M792-YB.
//
class M792Processor : public DeviceConfigProcessor
{
public:
	// Functions required by the DeviceConfigProcessor interface
	DeviceConfig getConfig () override;

private:
	M792Config m792Config {};

	// Define process as a pointer to a M792Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (M792Processor::* Process)(iniparser::Value);

	map<string, Process> valueProcessors =
	{
		{"option", &M792Processor::processOption}
	};

	void processOption (iniparser::Value value);

	// Private functions required by the SectionProcessor interface
	void processValue (iniparser::Section::ValueIterator valueIterator) override;
	void checkConsistency () override;
	void processSubsection (iniparser::Section* subSection) override;
};


#endif // _M792PROCESSOR_H_
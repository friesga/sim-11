#ifndef _RLV11PROCESSOR_H_
#define _RLV11PROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "configdata/rl/rlconfig/rlconfig.h"
#include "configdata/rl/rlprocessor/rlprocessor.h"

#include <map>
#include <string>

using std::map;
using std::string;

class RLV11Processor : public DeviceConfigProcessor
{
	RLV11Config rlv11Config;
	RLProcessor rlProcessor_;

	// Define process as a pointer to a RlProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RLV11Processor::* Process)(iniparser::Value);

	// As the RLV11 has no configuration parameters other than the common
	// RL configuration parameters.
	map<string, Process> valueProcessors =
	{};

	void processSection (iniparser::Section* section) override;
	void processValue (iniparser::Section::ValueIterator valueIterator) override;

	void checkConsistency () override;
	void processSubsection (iniparser::Section* subSection) override;

public:
	DeviceConfig getConfig () override;
};


#endif // !_RLV11PROCESSOR_H_

#ifndef _RLV12PROCESSOR_H_
#define _RLV12PROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "configdata/rl/rlconfig/rlconfig.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::string;

class RLV12Processor : public DeviceConfigProcessor
{
	unique_ptr<RLV12Config> rlConfigPtr {nullptr};

    // Define process as a pointer to a RlProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RLV12Processor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"controller", &RLV12Processor::processController},
		{"address", &RLV12Processor::processAddress},
		{"vector", &RLV12Processor::processVector},
		{"units", &RLV12Processor::processUnits},
		{"22-bit", &RLV12Processor::process22Bit}
	};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void processController (iniparser::Value value);
	void processAddress (iniparser::Value value);
	void processVector (iniparser::Value value);
	void processUnits (iniparser::Value value);
	void process22Bit (iniparser::Value value);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);

public:
	RLV12Processor ();
	DeviceConfig getConfig ();
};


#endif // !_RLV12PROCESSOR_H_

#ifndef _RLV12PROCESSOR_H_
#define _RLV12PROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "configdata/rl/rlconfig/rlconfig.h"
#include "configdata/rl/rlprocessor/rlprocessor.h"

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
	RLProcessor rlProcessor_;

    // Define process as a pointer to a RlProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RLV12Processor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"22-bit", &RLV12Processor::process22Bit}
	};

	void processSection (iniparser::Section* section) override;
    void processValue (iniparser::Section::ValueIterator valueIterator);
	void process22Bit (iniparser::Value value);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);

public:
	RLV12Processor ();
	DeviceConfig getConfig ();
};


#endif // !_RLV12PROCESSOR_H_

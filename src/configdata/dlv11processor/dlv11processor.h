#ifndef _DLV11PROCESSOR_H_
#define _DLV11PROCESSOR_H_

#include "../sectionprocessor/sectionprocessor.h"
#include "../dlv11config/dlv11config.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::map;
using std::string;

class DLV11Processor : public SectionProcessor
{
	unique_ptr<DLV11Config> dlv11ConfigPtr {nullptr};

    // Define process as a pointer to a BDV11Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (DLV11Processor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		// {"22-bit", &RLProcessor::process22Bit}
	};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);

public:
	DLV11Processor ();
	unique_ptr<DeviceConfig> getConfig ();
};


#endif // _DLV11PROCESSOR_H_
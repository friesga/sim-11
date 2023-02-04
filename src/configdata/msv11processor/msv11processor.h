#ifndef _MSV11PROCESSOR_H_
#define _MSV11PROCESSOR_H_

#include "../sectionprocessor/sectionprocessor.h"
#include "../msv11config/msv11config.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::map;
using std::string;

class MSV11Processor : public SectionProcessor
{
	unique_ptr<MSV11Config> msv11ConfigPtr {nullptr};

    // Define process as a pointer to a BDV11Processor member function
	// with a iniparser::Value argument and returning void.

	typedef void (MSV11Processor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		// {"22-bit", &RLProcessor::process22Bit}
	};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);

public:
	MSV11Processor ();
	unique_ptr<DeviceConfig> getConfig ();
};


#endif // _MSV11PROCESSOR_H_
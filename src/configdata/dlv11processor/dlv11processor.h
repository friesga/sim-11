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

    // Define process as a pointer to a DLV11Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (DLV11Processor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"address", &DLV11Processor::processAddress},
		{"vector", &DLV11Processor::processVector},
		{"ch3_console_enabled", &DLV11Processor::processConsoleEnabled},
		{"ch3_break_response", &DLV11Processor::processBreakResponse},
		{"break_key", &DLV11Processor::processBreakKey}
	};

	map<string, DLV11Config::Ch3BreakResponse> validBreakResponses =
	{
		{"boot", DLV11Config::Ch3BreakResponse::Boot},
		{"halt", DLV11Config::Ch3BreakResponse::Halt},
		{"none", DLV11Config::Ch3BreakResponse::None}
	};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void processAddress (iniparser::Value value);
	void processVector (iniparser::Value value);
	void processConsoleEnabled (iniparser::Value value);
	void processBreakResponse (iniparser::Value value);
	void processBreakKey (iniparser::Value value);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);

public:
	DLV11Processor ();
	unique_ptr<DeviceConfig> getConfig ();
};


#endif // _DLV11PROCESSOR_H_
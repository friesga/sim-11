#ifndef _DLV11PROCESSOR_H_
#define _DLV11PROCESSOR_H_

#include "configdata/sectionprocessor/sectionprocessor.h"
#include "configdata/serialconfig/dlv11jconfig/dlv11jconfig.h"
#include "configdata/serialconfig/uartconfig/uartconfig.h"

#include <memory>
#include <map>
#include <string>
#include <vector>

using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::string;
using std::vector;

class DLV11Processor : public SectionProcessor
{
public:
	DLV11Processor ();
	DeviceConfig getConfig ();

private:
	enum {numChannels = 4};
	enum {defaultCh3Address_ = 0177560};
	enum {defaultCh3Vector_ = 060};

	unique_ptr<DLV11JConfig> dlConfigPtr {nullptr};

    // Define process as a pointer to a DLV11Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (DLV11Processor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"address", &DLV11Processor::processAddress},
		{"vector", &DLV11Processor::processVector},
		{"ch3_console_enabled", &DLV11Processor::processConsoleEnabled},
		{"ch3_break_response", &DLV11Processor::processBreakResponse},
		{"break_key", &DLV11Processor::processBreakKey},
		{"loopback", &DLV11Processor::processLoopback}
	};

	map<string, ConsoleConfig::BreakResponse> validBreakResponses =
	{
		{"boot", ConsoleConfig::BreakResponse::Boot},
		{"halt", ConsoleConfig::BreakResponse::Halt},
		{"none", ConsoleConfig::BreakResponse::None}
	};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void processAddress (iniparser::Value value);
	void processVector (iniparser::Value value);
	void processConsoleEnabled (iniparser::Value value);
	void processBreakResponse (iniparser::Value value);
	void processBreakKey (iniparser::Value value);
	void processLoopback (iniparser::Value value);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);
	void checkBaseAddress ();
};


#endif // _DLV11PROCESSOR_H_
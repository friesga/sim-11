#ifndef _SLUPROCESSOR_H_
#define _SLUPROCESSOR_H_

#include "configdata/sectionprocessor/sectionprocessor.h"
#include "configdata/kdf11_b/sluconfig/sluconfig.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::string;

class SLUProcessor : public SectionProcessor
{
	unique_ptr<SLUConfig> sluConfigPtr {nullptr};

	typedef void (SLUProcessor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"slu1_enabled", &SLUProcessor::processSLU1Enabled},
		{"slu2_enabled", &SLUProcessor::processSLU2Enabled},
		{"slu2_address", &SLUProcessor::processSLU2Address},
		{"break_response", &SLUProcessor::processBreakResponse},
		{"break_key", &SLUProcessor::processBreakKey},
		{"loopback", &SLUProcessor::processLoopback}
	};

	map<string, ConsoleConfig::BreakResponse> validBreakResponses =
	{
		{"boot", ConsoleConfig::BreakResponse::Boot},
		{"halt", ConsoleConfig::BreakResponse::Halt},
		{"none", ConsoleConfig::BreakResponse::None}
	};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);
	void processSLU1Enabled (iniparser::Value value);
	void processSLU2Enabled (iniparser::Value value);
	void processSLUxEnabled (size_t unitNr, string id, iniparser::Value value);
	void processSLU2Address (iniparser::Value value);
	void processBreakResponse (iniparser::Value value);
	void processBreakKey (iniparser::Value value);
	void processLoopback (iniparser::Value value);

public:
	SLUProcessor ();
	DeviceConfigVariant getConfig ();
};


#endif // _SLUPROCESSOR_H_
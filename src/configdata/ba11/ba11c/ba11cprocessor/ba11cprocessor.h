#ifndef _BA11CPROCESSOR_H_
#define _BA11CPROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "configdata/ba11/ba11c/ba11cconfig/ba11cconfig.h"

#include <map>
#include <string>

using std::map;
using std::string;

class BA11_CProcessor : public DeviceConfigProcessor
{
	BA11_CConfig ba11_cConfig;

	// Define Process as a pointer to a BA11_CProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (BA11_CProcessor::* Process)(iniparser::Value);

	map<string, Process> valueProcessors =
	{
		{"cabinet", &BA11_CProcessor::processCabinet},
		{"console", &BA11_CProcessor::processOperatorConsole},
	};

	map<string, BA11_CConfig::OperatorConsole> availableConsoles =
	{
		{"KY11-A",     BA11_CConfig::OperatorConsole::KY11_A},
	};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section* subSection);
	void processCabinet (iniparser::Value value);
	void processOperatorConsole (iniparser::Value value);

public:
	DeviceConfig getConfig () override;
};

#endif // !_BA11CPROCESSOR_H_

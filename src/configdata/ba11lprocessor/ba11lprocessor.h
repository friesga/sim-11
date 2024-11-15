#ifndef _BA11LPROCESSOR_H_
#define _BA11LPROCESSOR_H_

#include "../sectionprocessor/deviceconfigprocessor.h"
#include "../ba11lconfig/ba11lconfig.h"

#include <memory>
#include <map>
#include <string>

using std::shared_ptr;
using std::unique_ptr;
using std::map;
using std::string;

class BA11_LProcessor : public DeviceConfigProcessor
{
	unique_ptr<BA11_LConfig> ba11_lConfigPtr {nullptr};

	// Define Process as a pointer to a BDV11Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (BA11_LProcessor::* Process)(iniparser::Value);

	map<string, Process> valueProcessors =
	{
		{"cabinet", &BA11_LProcessor::processCabinet},
	};


	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section* subSection);
	void processCabinet (iniparser::Value value);

public:
	BA11_LProcessor ();
	DeviceConfig getConfig () override;
};

#endif // !_BA11LPROCESSOR_H_

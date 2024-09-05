#ifndef _BA11NPROCESSOR_H_
#define _BA11NPROCESSOR_H_

#include "../sectionprocessor/deviceconfigprocessor.h"
#include "../ba11nconfig/ba11nconfig.h"

#include <memory>
#include <map>
#include <string>

using std::shared_ptr;
using std::unique_ptr;
using std::map;
using std::string;

class BA11_NProcessor : public DeviceConfigProcessor
{
	unique_ptr<BA11_NConfig> ba11_nConfigPtr {nullptr};

	// Define Process as a pointer to a BDV11Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (BA11_NProcessor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"logo", &BA11_NProcessor::processLogo},
		{"cabinet", &BA11_NProcessor::processCabinet},
	};

	map<string, BA11_NConfig::Logo> availableLogos =
	{
		{"PDP-11/03L",     BA11_NConfig::Logo::PDP_1103L},
		{"PDP-11/23",      BA11_NConfig::Logo::PDP_1123},
		{"PDP-11/23+",     BA11_NConfig::Logo::PDP_1123_PLUS}
	};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);
	void processLogo (iniparser::Value value);
	void processCabinet (iniparser::Value value);

public:
	BA11_NProcessor ();
	DeviceConfig getConfig () override;
};

#endif // !_BA11NPROCESSOR_H_

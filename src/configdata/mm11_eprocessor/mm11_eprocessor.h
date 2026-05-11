#ifndef _MM11EPROCESSOR_H_
#define _MM11EPROCESSOR_H_

#include "../sectionprocessor/deviceconfigprocessor.h"
#include "../mm11_econfig/mm11_econfig.h"

#include <map>
#include <string>

using std::map;
using std::string;

class MM11EProcessor : public DeviceConfigProcessor
{
	MM11EConfig mm11eConfig {};

	// Define process as a pointer to a MM11EProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (MM11EProcessor::* Process)(iniparser::Value);

	map<string, Process> valueProcessors =
	{
		{"starting_address", &MM11EProcessor::processStartingAddress},
		{"memory_extension", &MM11EProcessor::processMemoryExtension}
	};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section* subSection);
	void processStartingAddress (iniparser::Value value);
	void processMemoryExtension (iniparser::Value value);

public:
	DeviceConfig getConfig ();
};

#endif // _MM11EPROCESSOR_H_
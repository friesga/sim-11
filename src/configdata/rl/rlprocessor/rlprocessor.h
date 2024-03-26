#ifndef _RLPROCESSOR_H_
#define _RLPROCESSOR_H_

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "configdata/rl/rlconfig/rlconfig.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::string;

class RLProcessor : public DeviceConfigProcessor
{
	unique_ptr<RLConfig> rlConfigPtr {nullptr};

    // Define process as a pointer to a RlProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RLProcessor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"controller", &RLProcessor::processController},
		{"address", &RLProcessor::processAddress},
		{"vector", &RLProcessor::processVector},
		{"units", &RLProcessor::processUnits},
		{"22-bit", &RLProcessor::process22Bit}
	};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void processController (iniparser::Value value);
	void processAddress (iniparser::Value value);
	void processVector (iniparser::Value value);
	void processUnits (iniparser::Value value);
	void process22Bit (iniparser::Value value);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);

public:
	RLProcessor ();
	DeviceConfig getConfig ();
};


#endif // !_RLPROCESSOR_H_

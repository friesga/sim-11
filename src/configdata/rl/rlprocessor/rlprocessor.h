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

// Definition a configuration processor for options common to the RL11,
// RLV11 and RLV12.
class RLProcessor : public SectionProcessor
{
	unique_ptr<RLConfig> rlConfigPtr {nullptr};

	// Define process as a pointer to a RLProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RLProcessor::* Process)(iniparser::Value);

	map<string, Process> valueProcessors =
	{
		{"controller", &RLProcessor::processController},
		{"address", &RLProcessor::processAddress},
		{"vector", &RLProcessor::processVector},
		{"units", &RLProcessor::processUnits}
	};
	
	void processValue (iniparser::Section::ValueIterator valueIterator);
	void processController (iniparser::Value value);
	void processAddress (iniparser::Value value);
	void processVector (iniparser::Value value);
	void processUnits (iniparser::Value value);

public:
	RLProcessor ();
	void processSection (iniparser::Section* section);
	void processSubsection (iniparser::Section* subSection);
	void checkConsistency ();
	RLConfig getConfig ();
};

#endif // _RLPROCESSOR_H_
#ifndef _RLUNITPROCESSOR_H_
#define _RLUNITPROCESSOR_H_

#include "configdata/sectionprocessor/unitconfigprocessor.h"
#include "configdata/rl/rlunitconfig/rlunitconfig.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::string;

class RLUnitProcessor : public UnitConfigProcessor
{
	unique_ptr<RLUnitConfig> rlUnitConfigPtr {nullptr};

    // Define process as a pointer to a BDV11Processor member function
	// with a iniparser::Value argument and returning void.

	typedef void (RLUnitProcessor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"type", &RLUnitProcessor::processType},
		{"filename", &RLUnitProcessor::processFileName},
		{"newfile", &RLUnitProcessor::processNewFile},
		{"read-only", &RLUnitProcessor::processReadOnly},
		{"overwrite", &RLUnitProcessor::processOverwrite},
		{"cabinet", &RLUnitProcessor::processCabinet}
	};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);
	void processType (iniparser::Value value);
	void processFileName (iniparser::Value value);
	void processNewFile (iniparser::Value value);
	void processReadOnly (iniparser::Value value);
	void processOverwrite (iniparser::Value value);
	void processCabinet (iniparser::Value value);

public:
	RLUnitProcessor ();
	shared_ptr<RLUnitConfig> getConfig ();
};


#endif // _RLUNITPROCESSOR_H_
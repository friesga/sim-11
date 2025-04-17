#ifndef _RK05PROCESSOR_H_
#define _RK05PROCESSOR_H_

#include "configdata/sectionprocessor/unitconfigprocessor.h"
#include "configdata/rk/rk05/rk05config/rk05config.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::string;

class RK05Processor : public UnitConfigProcessor
{
	unique_ptr<RK05Config> rk05ConfigPtr {nullptr};

	// Define process as a pointer to a RK05Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RK05Processor::* Process)(iniparser::Value);

	map<string, Process> valueProcessors =
	{
		{"filename", &RK05Processor::processFileName},
		{"newfile", &RK05Processor::processNewFile},
		{"write-protect", &RK05Processor::processWriteProtect},
		{"overwrite", &RK05Processor::processOverwrite},
		{"cabinet", &RK05Processor::processCabinet},
		{"spin-up-time", &RK05Processor::processSpinUpTime}
	};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section* subSection);
	void processFileName (iniparser::Value value);
	void processNewFile (iniparser::Value value);
	void processWriteProtect (iniparser::Value value);
	void processOverwrite (iniparser::Value value);
	void processCabinet (iniparser::Value value);
	void processSpinUpTime (iniparser::Value value);

public:
	RK05Processor (size_t unitNumber);
	shared_ptr<RK05Config> getConfig ();
};

#endif // _RK05PROCESSOR_H_
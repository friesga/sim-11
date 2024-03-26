#ifndef _RXV21UNITPROCESSOR_H_
#define _RXV21UNITPROCESSOR_H_

#include "configdata/sectionprocessor/unitconfigprocessor.h"
#include "configdata/rxv21/rxv21unitconfig/rxv21unitconfig.h"
#include "rxv21unitprocessor.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::string;

class RXV21UnitProcessor : public UnitConfigProcessor
{
	unique_ptr<RXV21UnitConfig> rxv21UnitConfigPtr {nullptr};

    // Define process as a pointer to a BDV11Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RXV21UnitProcessor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"filename", &RXV21UnitProcessor::processFileName},
		{"newfile", &RXV21UnitProcessor::processNewFile},
		{"read-only", &RXV21UnitProcessor::processReadOnly},
		{"overwrite", &RXV21UnitProcessor::processOverwrite}
	};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);
	void processFileName (iniparser::Value value);
	void processNewFile (iniparser::Value value);
	void processReadOnly (iniparser::Value value);
	void processOverwrite (iniparser::Value value);

public:
	RXV21UnitProcessor ();
	shared_ptr<RXV21UnitConfig> getConfig ();
};


#endif // _RXV21UNITPROCESSOR_H_
#ifndef _RLUNITPROCESSOR_H_
#define _RLUNITPROCESSOR_H_

#include "../../iniparser.h"
#include <string>

// Processor for the unit subsection of an RL section
class RlUnitProcessor 
{
	bool RL01 {true};
	std::string fileName;
	bool newFile {false};
	bool readOnly {false};

	// Define process as a pointer to a RlProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RlUnitProcessor::*Process)(iniparser::Value);
	
	std::map<std::string, Process> processValue =
	{
		{"type", &RlUnitProcessor::processType},
		{"filename", &RlUnitProcessor::processFileName},
		{"newfile", &RlUnitProcessor::processNewFile},
		{"read-only", &RlUnitProcessor::processReadOnly}
	};

	void processType (iniparser::Value value);
	void processFileName (iniparser::Value value);
	void processNewFile (iniparser::Value value);
	void processReadOnly (iniparser::Value value);
	size_t unitNumberFromSectionName (std::string);

public:
	void processSection (iniparser::Section* section);
};

#endif // _RLUNITPROCESSOR_H_
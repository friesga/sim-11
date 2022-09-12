#ifndef _RLUNITPROCESSOR_H_
#define _RLUNITPROCESSOR_H_

#include "../../iniparser.h"
#include "../../include/configdata.h"

// Processor for the unit subsection of an RL section
class RlUnitProcessor 
{
	// Define the unit configuration and the unit number to which that
	// configuration applies.
	RlUnitConfig rlUnitConfig;
	size_t unitNumber_;

	// Define process as a pointer to a RlProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RlUnitProcessor::*Process)(iniparser::Value);
	
	std::map<std::string, Process> processValue =
	{
		{"type", &RlUnitProcessor::processType},
		{"filename", &RlUnitProcessor::processFileName},
		{"newfile", &RlUnitProcessor::processNewFile},
		{"read-only", &RlUnitProcessor::processReadOnly},
		{"overwrite", &RlUnitProcessor::processOverwrite}
	};

	void processType (iniparser::Value value);
	void processFileName (iniparser::Value value);
	void processNewFile (iniparser::Value value);
	void processReadOnly (iniparser::Value value);
	void processOverwrite (iniparser::Value value);
	size_t unitNumberFromSectionName (std::string);

public:
	void processSection (iniparser::Section* section);
	size_t getUnitNumber();
	RlUnitConfig getConfig ();
};

#endif // _RLUNITPROCESSOR_H_
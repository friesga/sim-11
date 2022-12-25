#ifndef _RXUNITPROCESSOR_H_
#define _RXUNITPROCESSOR_H_

#include "../../iniparser.h"
#include "../../include/configdata.h"

// Processor for the unit subsection of an RX section
class RxUnitProcessor 
{
	// Define the unit configuration and the unit number to which that
	// configuration applies.
	RxUnitConfig rxUnitConfig;
	size_t unitNumber_;

	// Define process as a pointer to a RlProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RxUnitProcessor::*Process)(iniparser::Value);
	
	std::map<std::string, Process> processValue =
	{
		{"filename", &RxUnitProcessor::processFileName},
	};

	void processFileName (iniparser::Value value);
	size_t unitNumberFromSectionName (std::string);

public:
	void processSection (iniparser::Section* section);
	void checkConsistency ();
	size_t getUnitNumber();
	RxUnitConfig getConfig ();
};

#endif // _RXUNITPROCESSOR_H_
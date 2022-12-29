#ifndef _RX_PROCESSOR_H_
#define _RX_PROCESSOR_H_

#include "../include/configdata.h"
#include "../iniparser.h"
#include "../sectionprocessor/sectionprocessor.h"
#include "rxunitprocessor/rxunitprocessor.h"

// Processor for the RX section of a configuration file
class RxProcessor : public SectionProcessor
{
	std::unique_ptr<RxConfig> rxConfigPtr {nullptr};

	// Define process as a pointer to a RlProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RxProcessor::*Process)(iniparser::Value);
	
	std::map<std::string, Process> processValue =
	{
		{"address", &RxProcessor::processAddress},
		{"vector", &RxProcessor::processVector}
	};

	void processAddress (iniparser::Value value);
	void processVector (iniparser::Value value);

	void checkConsistency();

public:
	void processSection (iniparser::Section* section) override;
	RxConfig *getConfig();
};

#endif // _RXPROCESSOR_H_
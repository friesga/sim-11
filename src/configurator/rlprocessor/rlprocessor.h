#ifndef _RL_PROCESSOR_H_
#define _RL_PROCESSOR_H_

#include "../iniparser.h"
#include "../sectionprocessor/sectionprocessor.h"

// Processor for the RL section of a configuration file
class RlProcessor : public SectionProcessor
{
	uint16_t address {0174400};
	uint16_t vector {0160};
	size_t numUnits {1};
	bool RLV11 {true};

	// Define process as a pointer to a RlProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RlProcessor::*Process)(iniparser::Value);
	
	std::map<std::string, Process> processValue =
	{
		{"controller", &RlProcessor::processController},
		{"address", &RlProcessor::processAddress},
		{"vector", &RlProcessor::processVector},
		{"units", &RlProcessor::processUnits}
	};

	void processController (iniparser::Value value);
	void processAddress (iniparser::Value value);
	void processVector (iniparser::Value value);
	void processUnits (iniparser::Value value);
	uint16_t touint16_t (std::string number);

public:
	void processSection (iniparser::Section* section) override;
};

#endif // _RLPROCESSOR_H_
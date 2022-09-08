#ifndef _RL_PROCESSOR_H_
#define _RL_PROCESSOR_H_

#include "../iniparser.h"
#include "../sectionprocessor/sectionprocessor.h"
#include "rlunitprocessor/rlunitprocessor.h"

struct RlConfig
{
	static constexpr size_t maxRlUnits = 4;
	uint16_t address {0};
	uint16_t vector {0};
	size_t numUnits {0};
	bool rlv11 {false};
	bool _22bit {false};
	RlUnitConfig rlUnitConfig[maxRlUnits];
};

// Processor for the RL section of a configuration file
class RlProcessor : public SectionProcessor
{
	std::unique_ptr<RlConfig> rlConfigPtr = std::make_unique<RlConfig> ();

	// Define process as a pointer to a RlProcessor member function
	// with a iniparser::Value argument and returning void.
	typedef void (RlProcessor::*Process)(iniparser::Value);
	
	std::map<std::string, Process> processValue =
	{
		{"controller", &RlProcessor::processController},
		{"address", &RlProcessor::processAddress},
		{"vector", &RlProcessor::processVector},
		{"units", &RlProcessor::processUnits},
		{"22-bit", &RlProcessor::process22Bit}
	};

	void processController (iniparser::Value value);
	void processAddress (iniparser::Value value);
	void processVector (iniparser::Value value);
	void processUnits (iniparser::Value value);
	void process22Bit (iniparser::Value value);
	uint16_t touint16_t (std::string number);

	void checkConsistency();

public:
	void processSection (iniparser::Section* section) override;
	RlConfig *getConfig();
};

#endif // _RLPROCESSOR_H_
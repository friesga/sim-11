#ifndef _RL_PROCESSOR_H_
#define _RL_PROCESSOR_H_

#include "../iniparser.h"
#include "../sectionprocessor/sectionprocessor.h"
#include "rlunitprocessor/rlunitprocessor.h"

// Three type of RL controllers exist:
// - RL11. The RL11 controller is used to interface with the Unibus,
// - RLV11. This controller interfaces the drive with the LSI-11 bus,
//   i.e. the 18-bit Qbus,
// - RLV12. It is used to interface the drive with the LSI-11 (18-bit) or
//   Extended LSI-11 (22-bit) Qbus.
// 
// Source: RL01/RL02 User Guide (EK-RL012-UG-005)
//
struct RlConfig
{
	enum class RLType
	{	
		RL11,
		RLV11,
		RLV12
	};

	static constexpr size_t maxRlUnits = 4;
	uint16_t address {0};
	uint16_t vector {0};
	size_t numUnits {0};
	RLType rlType {RLType::RLV12};
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
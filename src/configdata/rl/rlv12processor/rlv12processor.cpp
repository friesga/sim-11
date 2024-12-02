#include "rlv12processor.h"
#include "configdata/rl/rlprocessor/rlprocessor.h"
#include "configdata/rl/rlunitconfig/rlunitconfig.h"
#include "configdata/rl/rlunitprocessor/rlunitprocessor.h"
#include "touint.h"

#include <utility>

using std::make_unique;
using std::move;

RLV12Processor::RLV12Processor ()
{
	rlConfigPtr = make_unique<RLV12Config> ();
}

// This is a RLV12-specific processSection(). This version first calls the
// RL configuration processor to process the common keys and then calls the
// regular ProcessSection() to process the remaining keys. Common keys are
// deleted from the section so they aren't processed by the regular
// ProcessSection().
void RLV12Processor::processSection (iniparser::Section* section)
{
	RLProcessor rlProcessor;
	rlProcessor.processSection (section);
	rlConfigPtr->common = rlProcessor.getConfig ();

	SectionProcessor::processSection (section);
}

void RLV12Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    // Throw exception for non-existing key?
    Process processFunction = valueProcessors[valueIterator->first];
    (this->*processFunction)(valueIterator->second);
}

// Explicitly test for "true" and "false" as AsBool() returns no error,
// only checks the first character and returns false except for strings parsed
// as true.
void RLV12Processor::process22Bit (iniparser::Value value)
{
	if (value.asString() == "true")
		rlConfigPtr->_22bit = true;
	else if (value.asString() == "false")
		rlConfigPtr->_22bit = false;
	else
		throw std::invalid_argument {"22-bit must be either true or false"};
}

// Check the consistency of the configuration of the RLV1[12] controller
// 
// Currently the simulator only supports the Qbus and therefore the RLV11 
// and RLV12 controllers. When the Unibus is supported the configurator
// should check the controller/bus type consistency.
// 
// The RLV12 controller contains a M1-M2 jumper which, when installed, enables
// the 22-bit option. This option is not present on the RLV11, so allow this
// option only if the controller type is RLV12. 
void RLV12Processor::checkConsistency ()
{
	if (rlConfigPtr->common.rlType == RLConfig::RLType::RL11)
		throw std::invalid_argument 
			{"The RL11 can only be configured on Unibus systems"};

	if (rlConfigPtr->_22bit && rlConfigPtr->common.rlType != RLConfig::RLType::RLV12)
		throw std::invalid_argument 
			{"The 22-bit option is only allowed on an RLV12 controller"};
}

// A RL Section can have zero to four subsections, one for each unit.
void RLV12Processor::processSubsection (iniparser::Section *subSection)
{
	if (subSection->name().substr(0, 4) != "unit")
		throw std::invalid_argument {"Unknown RL subsection: " + 
			subSection->name()};

	// Get the unit number from the subsection name. This will throw an
	// exception if an incorrect unit number is specified. The unit number
	// is stored in the RlUnitConfig struct so it is clear to which unit
	// the configuration applies.
	size_t unitNumber = unitNumberFromSectionName (subSection->name(),
		rlConfigPtr->common.maxRlUnits);

	// Check that the configuration for this unit has not already been
	// specified.
	if (rlConfigPtr->common.rlUnitConfig[unitNumber] != nullptr)
		throw std::invalid_argument {"Double specification for RL subsection: " + 
			subSection->name()};

	RLUnitProcessor rlUnitProcessor {unitNumber};
	rlUnitProcessor.processSection (subSection);

	// Add the unit configuration to the RL device configuration
	rlConfigPtr->common.rlUnitConfig[unitNumber] = rlUnitProcessor.getConfig ();
}

DeviceConfig RLV12Processor::getConfig ()
{
	return move (rlConfigPtr);
}
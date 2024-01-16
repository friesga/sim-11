#include "rlprocessor.h"
#include "../rlunitconfig/rlunitconfig.h"
#include "../rlunitprocessor/rlunitprocessor.h"

#include <utility>

using std::make_unique;
using std::move;

RLProcessor::RLProcessor ()
{
	rlConfigPtr = make_unique<RLConfig> ();
}

void RLProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    // Throw exception for non-existing key?
    Process processFunction = valueProcessors[valueIterator->first];
    (this->*processFunction)(valueIterator->second);
}

// 
// Determine the controller type, either RLV11 or RLV12.
// 
void RLProcessor::processController (iniparser::Value value)
{
	if (value.asString() == "RL11")
		rlConfigPtr->rlType = RLConfig::RLType::RL11;
	else if (value.asString() == "RLV11")
		rlConfigPtr->rlType = RLConfig::RLType::RLV11;
	else if (value.asString() == "RLV12")
		rlConfigPtr->rlType = RLConfig::RLType::RLV12;
	else
		throw std::invalid_argument {"Incorrect RL controller type: " + 
			value.asString()};
}

void RLProcessor::processAddress (iniparser::Value value)
{
	try
	{
		rlConfigPtr->address = touint<u16> (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"Incorrect address in RL section specified: " + 
			value.asString()};
	}
}

void RLProcessor::processVector (iniparser::Value value) 
{ 
	try
	{
		rlConfigPtr->vector = touint<u16> (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"Incorrect vector in RL section specified: " + 
			value.asString()};
	}
}

void RLProcessor::processUnits (iniparser::Value value)
{
	rlConfigPtr->numUnits = value.asInt ();
}

// Explicitly test for "true" and "false" as AsBool() returns no error,
// only checks the first character and returns false except for strings parsed
// as true.
void RLProcessor::process22Bit (iniparser::Value value)
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
void RLProcessor::checkConsistency ()
{
	if (rlConfigPtr->rlType == RLConfig::RLType::RL11)
		throw std::invalid_argument 
			{"The RL11 can only be configured on Unibus systems"};

	if (rlConfigPtr->_22bit && rlConfigPtr->rlType != RLConfig::RLType::RLV12)
		throw std::invalid_argument 
			{"The 22-bit option is only allowed on an RLV12 controller"};
}

// A RL Section can have zero to four subsections, one for each unit.
void RLProcessor::processSubsection (iniparser::Section *subSection)
{
	if (subSection->name().substr(0, 4) != "unit")
		throw std::invalid_argument {"Unknown RL subsection: " + 
			subSection->name()};

	// Get the unit number from the subsection name. This will throw an
	// exception if an incorrect unit number is specified. The unit number
	// is stored in the RlUnitConfig struct so it is clear to which unit
	// the configuration applies.
	size_t unitNumber = unitNumberFromSectionName (subSection->name());

	// Check that the configuration for this unit has not already been
	// specified.
	if (rlConfigPtr->rlUnitConfig[unitNumber] != nullptr)
		throw std::invalid_argument {"Double specification for RL subsection: " + 
			subSection->name()};

	RLUnitProcessor rlUnitProcessor {};
	rlUnitProcessor.processSection (subSection);

	// Add the unit configuration to the Rl device configuration
	rlConfigPtr->rlUnitConfig[unitNumber] = rlUnitProcessor.getConfig ();
}

unique_ptr<DeviceConfig> RLProcessor::getConfig ()
{
	return move (rlConfigPtr);
}
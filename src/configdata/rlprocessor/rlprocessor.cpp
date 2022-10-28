#include "rlprocessor.h"
#include "rlunitprocessor/rlunitprocessor.h"

#include <limits>

// Process the given RL section
void RlProcessor::processSection (iniparser::Section* section)
{
	// Process section's Values (i.e. key/value pairs)
	for (iniparser::Section::ValueIterator valueIterator = section->valuesBegin();
			valueIterator != section->valuesEnd(); ++valueIterator)
	{
		try
		{
			Process processFunction = processValue.at (valueIterator->first);
			(this->*processFunction)(valueIterator->second);
		}
		catch (std::out_of_range const &)
		{
			throw std::invalid_argument {"Unknown key in section RL: " +  
				valueIterator->first};
		}
	}

	// Check the consistency of the specified RL options
	checkConsistency();

	// Process subsections
	// A subsection name must start with the string "unit", followed by
	// a unit number in the range 0-4.
	for (iniparser::Section* subSectionPtr : section->findSubSections())
	{
		RlUnitProcessor rlUnitProcessor;

		rlUnitProcessor.processSection (subSectionPtr);

		// Get the configuration for this unit and store it in the RL
		// configuration
		size_t unitNumber = rlUnitProcessor.getUnitNumber();
		rlConfigPtr->rlUnitConfig[unitNumber] = rlUnitProcessor.getConfig();

		// If a unit number higher than the specified number of units is
		// used generate an error
		if (unitNumber >= rlConfigPtr->numUnits)
			throw std::invalid_argument {"Unit number " + 
			std::to_string(unitNumber) + " is higher than specified #units (" +
			std::to_string(rlConfigPtr->numUnits) + ") in RL section"};

		// Check the consistency of this section
		rlUnitProcessor.checkConsistency ();
	}
}

RlConfig *RlProcessor::getConfig()
{
	return rlConfigPtr.get();
}

// Determine the controller type, either RLV11 or RLV12.
// 
// 

void RlProcessor::processController (iniparser::Value value)
{
	if (value.asString() == "RL11")
		rlConfigPtr->rlType = RlConfig::RLType::RL11;
	else if (value.asString() == "RLV11")
		rlConfigPtr->rlType = RlConfig::RLType::RLV11;
	else if (value.asString() == "RLV12")
		rlConfigPtr->rlType = RlConfig::RLType::RLV12;
	else
		throw std::invalid_argument {"Incorrect RL controller type: " + 
			value.asString()};
}

void RlProcessor::processAddress (iniparser::Value value)
{
	try
	{
		rlConfigPtr->address = touint16_t (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"Incorrect address in RL section specified: " + 
			value.asString()};
	}
}

void RlProcessor::processVector (iniparser::Value value) 
{ 
	try
	{
		rlConfigPtr->vector = touint16_t (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"Incorrect vector in RL section specified: " + 
			value.asString()};
	}
}

void RlProcessor::processUnits (iniparser::Value value)
{
	rlConfigPtr->numUnits = value.asInt ();
}

// Explicitly test for "true" and "false" as AsBool() returns no error,
// only checks the first character and returns false except for strings parsed
// as true.
void RlProcessor::process22Bit (iniparser::Value value)
{
	if (value.asString() == "true")
		rlConfigPtr->_22bit = true;
	else if (value.asString() == "false")
		rlConfigPtr->_22bit = false;
	else
		throw std::invalid_argument {"22-bit must be either true or false"};
}

// Return the number represented by the string as an uint16_t. The number's 
// base is determined by stoul. This allows for the specification of an
// address as an octal numer.
uint16_t RlProcessor::touint16_t (std::string number)
{
	size_t numConvertedChars;
	uint32_t tmp = stoul (number, &numConvertedChars, 0);

	if (numConvertedChars != number.size() || 
		tmp > std::numeric_limits<uint16_t>::max())
			throw std::invalid_argument{number};

	return tmp;
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
void RlProcessor::checkConsistency ()
{
	if (rlConfigPtr->rlType == RlConfig::RLType::RL11)
		throw std::invalid_argument 
			{"The RL11 can only be configured on Unibus systems"};

	if (rlConfigPtr->_22bit && rlConfigPtr->rlType != RlConfig::RLType::RLV12)
		throw std::invalid_argument 
			{"The 22-bit option is only allowed on an RLV12 controller"};
}

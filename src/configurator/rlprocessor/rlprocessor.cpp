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
	}
}

RlConfig *RlProcessor::getConfig()
{
	return rlConfigPtr.get();
}

// Determine the controller type, either RLV11 or RLV12.
// 
// Three type of RL controllers exist:
// - RL11. The RL11 controller is used to interface with the Unibus,
// - RLV11. This controller interfaces the drive with the LSI-11 bus,
//   i.e. the 18-bit Qbus,
// - RLV12. It is used to interface the drive with the LSI-11 (18-bit) or
//   Extended LSI-11 (22-bit) Qbus.
// 
// Source: RL01/RL02 User Guide (EK-RL012-UG-005)
// 
// Currently the simulator only supports the Qbus and therefore the RLV11 
// and RLV12 controllers. When the Unibus is supported the configurator
// should check the controller/bus type consistency.
void RlProcessor::processController (iniparser::Value value)
{
	if (value.asString() == "RLV11")
		rlConfigPtr->RLV11 = true;
	else if (value.asString() == "RLV12")
		rlConfigPtr->RLV11 = false;
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

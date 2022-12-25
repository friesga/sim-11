#include "rxprocessor.h"
#include "rxunitprocessor/rxunitprocessor.h"

#include <limits>

// Process the given RX section
void RxProcessor::processSection (iniparser::Section* section)
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
				section->fullName()};
		}
	}

	// Check the consistency of the specified RL options
	checkConsistency();

	// Process subsections
	// A subsection name must start with the string "unit", followed by
	// a unit number in the range 0-4.
	for (iniparser::Section* subSectionPtr : section->findSubSections())
	{
		RxUnitProcessor rxUnitProcessor;

		rxUnitProcessor.processSection (subSectionPtr);

		// Get the configuration for this unit and store it in the RL
		// configuration
		size_t unitNumber = rxUnitProcessor.getUnitNumber();
		rxConfigPtr->rxUnitConfig[unitNumber] = rxUnitProcessor.getConfig();

		// If a unit number higher than the specified number of units is
		// used generate an error
		if (unitNumber >= rxConfigPtr->numRxUnits)
			throw std::invalid_argument {"Unit number " + 
			std::to_string(unitNumber) + " is higher than maximum #units (" +
			std::to_string(rxConfigPtr->numRxUnits) + ") in RL section"};

		// Check the consistency of this section
		rxUnitProcessor.checkConsistency ();
	}
}

RxConfig *RxProcessor::getConfig()
{
	return rxConfigPtr.get();
}

void RxProcessor::processAddress (iniparser::Value value)
{
	try
	{
		rxConfigPtr->address = touint16_t (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"Incorrect address in RL section specified: " + 
			value.asString()};
	}
}

void RxProcessor::processVector (iniparser::Value value) 
{ 
	try
	{
		rxConfigPtr->vector = touint16_t (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"Incorrect vector in RX section specified: " + 
			value.asString()};
	}
}

// Check the consistency of the configuration of the RX controller. Currently
// this is a void.
void RxProcessor::checkConsistency ()
{
}

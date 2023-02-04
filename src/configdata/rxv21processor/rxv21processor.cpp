#include "rxv21processor.h"
#include "../rxv21unitprocessor/rxv21unitprocessor.h"

#include <utility>

using std::make_unique;
using std::move;

RXV21Processor::RXV21Processor ()
{
	rxConfigPtr = make_unique<RXV21Config> ();
}

void RXV21Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    // Throw exception for non-existing key?
    Process processFunction = valueProcessors[valueIterator->first];
    (this->*processFunction)(valueIterator->second);
}

void RXV21Processor::processAddress (iniparser::Value value)
{
	try
	{
		rxConfigPtr->address = touint16_t (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"Incorrect address in RX section specified: " + 
			value.asString()};
	}
}

void RXV21Processor::processVector (iniparser::Value value) 
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
void RXV21Processor::checkConsistency ()
{
}

// Process a section with a unit specification.
void RXV21Processor::processSubsection (iniparser::Section *subSection)
{
	if (subSection->name().substr(0, 4) != "unit")
		throw std::invalid_argument {"Unknown RXV21 subsection: " + 
			subSection->name()};

	// Get the unit number from the subsection name. This will throw an
	// exception if an incorrect unit number is specified. The unit number
	// is stored in the RXV21UnitConfig struct so it is clear to which unit
	// the configuration applies.
	size_t unitNumber = unitNumberFromSectionName (subSection->name());

	// Check that the configuration for this unit has not already been
	// specified.
	if (rxConfigPtr->rxv21UnitConfig[unitNumber] != nullptr)
		throw std::invalid_argument {"Double specification for RXV21 subsection: " + 
			subSection->name()};

	RXV21UnitProcessor rxv21UnitProcessor {};
	rxv21UnitProcessor.processSection (subSection);

	// Add the unit configuration to the RXV21 device configuration
	rxConfigPtr->rxv21UnitConfig[unitNumber] = rxv21UnitProcessor.getConfig ();
}

unique_ptr<DeviceConfig> RXV21Processor::getConfig ()
{
	return move (rxConfigPtr);
}
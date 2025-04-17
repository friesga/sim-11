#include "rk11dprocessor.h"
#include "configdata/rk/rk05/rk05config/rk05config.h"
#include "configdata/rk/rk05/rk05processor/rk05processor.h"
#include "touint.h"

#include <utility>

using std::make_unique;
using std::move;

RK11DProcessor::RK11DProcessor ()
{
	rk11dConfigPtr = make_unique<RK11DConfig> ();
}

void RK11DProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
		// This exception will be catched and processed in 
		// SectionProcessor::processSection().
		throw std::out_of_range ("Unknown key in RK11-D section");

	(this->*processFunction)(valueIterator->second);
}

void RK11DProcessor::processAddress (iniparser::Value value)
{
	u16 address {0};

	try
	{
		address = touint<u16> (value.asString ());
	}
	catch (std::invalid_argument const&)
	{
		throw std::invalid_argument {"Incorrect address in RK11-D section specified: " +
			value.asString ()};
	}

	if (address & 07)
		throw std::invalid_argument {"RK11-D address must be on a eight byte boundary"};

	if (address < 0160000 || address > 0177770)
		throw std::invalid_argument {"RK11-D address must be in the range 0160000-0177770"};

	rk11dConfigPtr->address = address;
}

void RK11DProcessor::processVector (iniparser::Value value)
{
	u16 vector {0};

	try
	{
		vector = touint<u16> (value.asString ());
	}
	catch (std::invalid_argument const&)
	{
		throw std::invalid_argument {"Incorrect vector in RK11-D section specified: " +
			value.asString ()};
	}

	if (vector & 1)
        throw std::invalid_argument {"RK11-D vector must be even"};

	if (vector > 0740)
        throw std::invalid_argument {"RK11-D vector must be in the range 0000-0740"};

	rk11dConfigPtr->vector = vector;
}

void RK11DProcessor::processBRLevel (iniparser::Value value)
{
	u8 busRequestLevel {0};

	try
	{
		busRequestLevel = touint<u16> (value.asString ());
	}
	catch (std::invalid_argument const&)
	{
		throw std::invalid_argument {"Incorrect bus request level in RK11-D section specified: " +
			value.asString ()};
	}

	if (busRequestLevel < 4 || busRequestLevel > 7)
        throw std::invalid_argument {"RK11-D bus request level must be between 4 and 7"};

	rk11dConfigPtr->busRequestLevel = busRequestLevel;
}

void RK11DProcessor::processUnits (iniparser::Value value)
{
	rk11dConfigPtr->numUnits = value.asInt ();
}

// A RL Section can have zero to four subsections, one for each unit.
void RK11DProcessor::processSubsection (iniparser::Section* subSection)
{
	if (subSection->name ().substr (0, 5) != "RK05-")
		throw std::invalid_argument {"Unknown RK11-D subsection: " +
			subSection->name ()};

	// Get the unit number from the subsection name. This will throw an
	// exception if an incorrect unit number is specified. The unit number
	// is stored in the RlUnitConfig struct so it is clear to which unit
	// the configuration applies.
	size_t unitNumber = unitNumberFromSectionName (subSection->name (),
		rk11dConfigPtr->maxRK05Units);

	// Check that the configuration for this unit has not already been
	// specified.
	if (rk11dConfigPtr->rk05Config[unitNumber] != nullptr)
		throw std::invalid_argument {"Double specification for RK11-D subsection: " +
			subSection->name ()};

	RK05Processor rk05Processor {unitNumber};
	rk05Processor.processSection (subSection);

	// Add the unit configuration to the RL device configuration
	rk11dConfigPtr->rk05Config[unitNumber] = rk05Processor.getConfig ();
}

void RK11DProcessor::checkConsistency ()
{
}

DeviceConfig RK11DProcessor::getConfig ()
{
	return move (rk11dConfigPtr);
}
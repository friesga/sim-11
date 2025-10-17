#include "rk11dprocessor.h"
#include "configdata/rk/rk05/rk05config/rk05config.h"
#include "configdata/rk/rk05/rk05processor/rk05processor.h"
#include "touint.h"

#include <utility>

using std::make_unique;
using std::move;
using std::to_string;

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

	rk11dConfig.address = address;
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

	rk11dConfig.vector = vector;
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

	rk11dConfig.busRequestLevel = busRequestLevel;
}

void RK11DProcessor::processUnits (iniparser::Value value)
{
	rk11dConfig.numUnits = value.asInt ();
}

// A RL Section can have zero to four subsections, one for each unit.
// The unit number is specified by the mandatory unit key. After use in this
// function, the unit number is removed from the section to avoid it being
// processed again by the SectionProcessor base class.
void RK11DProcessor::processSubsection (iniparser::Section* subSection)
{
	size_t unitNumber;

	// Get the unit number from the unit key in this subsection.
	iniparser::Value unitValue = subSection->getValue ("unit");

	if (!unitValue.isValid ())
		throw std::invalid_argument {"Unit number not specified in section: " +
			subSection->fullName ()};

    // Remove the unit key from the subsection
    subSection->removeValue ("unit");

	// The unit number is stored in the RlUnitConfig struct so it is clear
	// to which unit the configuration applies.
	try
	{
		unitNumber = unitValue.asInt ();
	}
	catch (std::invalid_argument const&)
    {
        throw std::invalid_argument {"Invalid unit number in section " +
			subSection->fullName ()};
    }

    // Check validity of the unit number
	if (unitNumber >= rk11dConfig.maxRK05Units)
		throw std::invalid_argument {"Unit number out of range 0-" +
			to_string (rk11dConfig.maxRK05Units - 1) + " in section " + subSection->fullName ()};

	// Check that the configuration for this unit has not already been
	// specified.
	if (rk11dConfig.rk05Config[unitNumber].has_value ())
		throw std::invalid_argument {"Double specification for RK11-D subsection: " +
			subSection->name ()};

	RK05Processor rk05Processor {unitNumber};
	rk05Processor.processSection (subSection);

	// Add the unit configuration to the RL device configuration
	rk11dConfig.rk05Config[unitNumber] = rk05Processor.getConfig ();
}

void RK11DProcessor::checkConsistency ()
{
}

DeviceConfig RK11DProcessor::getConfig ()
{
	return rk11dConfig;
}
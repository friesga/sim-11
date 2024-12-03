#include "rlprocessor.h"
#include "configdata/rl/rlunitconfig/rlunitconfig.h"
#include "configdata/rl/rlunitprocessor/rlunitprocessor.h"
#include "touint.h"

#include <utility>

using std::make_unique;
using std::move;

RLProcessor::RLProcessor ()
{
	rlConfigPtr = make_unique<RLConfig> ();
}

// This is an overridden version of SectionProcessor::processSection() with
// the following differences:
// - Unknown keys are ignored,
// - After a known key is processed it is removed from the section,
//
// This version of processSection can be used to process a section with keys
// common to multiple devices (such as the RL11, RLV11 and RLV12). The
// configuration processor for these devices first calls
// RLProcessor::processSection() to process the common keys. The remaining
// keys in the section are then either device-specific or unknown keys.
//
void RLProcessor::processSection (iniparser::Section* section)
{
	// Process section's Values (i.e. key/value pairs)
	for (iniparser::Section::ValueIterator valueIterator = section->valuesBegin ();
		valueIterator != section->valuesEnd ();)
	{
		// Erasing an entry in a map causes the iterator to be invalidated.
		// We therefore make an explicit copy of the iterator, increment it
		// and remove the entry using the copy of the constructor. See e.g.:
		// https://www.techiedelight.com/remove-entries-map-iterating-cpp/
		auto currentIterator = valueIterator++;

		if (valueProcessors.find (currentIterator->first) != valueProcessors.end ())
		{
			processValue (currentIterator); 
			section->removeValue (currentIterator->first);
		}
	}
}

void RLProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];
	(this->*processFunction)(valueIterator->second);
}

void RLProcessor::processAddress (iniparser::Value value)
{
	try
	{
		rlConfigPtr->address = touint<u16> (value.asString ());
	}
	catch (std::invalid_argument const&)
	{
		throw std::invalid_argument {"Incorrect address in RL section specified: " +
			value.asString ()};
	}
}

void RLProcessor::processVector (iniparser::Value value)
{
	try
	{
		rlConfigPtr->vector = touint<u16> (value.asString ());
	}
	catch (std::invalid_argument const&)
	{
		throw std::invalid_argument {"Incorrect vector in RL section specified: " +
			value.asString ()};
	}
}

void RLProcessor::processUnits (iniparser::Value value)
{
	rlConfigPtr->numUnits = value.asInt ();
}

// A RL Section can have zero to four subsections, one for each unit.
void RLProcessor::processSubsection (iniparser::Section* subSection)
{
	if (subSection->name().substr(0, 4) != "unit")
		throw std::invalid_argument {"Unknown RL subsection: " +
			subSection->name()};

	// Get the unit number from the subsection name. This will throw an
	// exception if an incorrect unit number is specified. The unit number
	// is stored in the RlUnitConfig struct so it is clear to which unit
	// the configuration applies.
	size_t unitNumber = unitNumberFromSectionName (subSection->name(),
		rlConfigPtr->maxRlUnits);

	// Check that the configuration for this unit has not already been
	// specified.
	if (rlConfigPtr->rlUnitConfig[unitNumber] != nullptr)
		throw std::invalid_argument {"Double specification for RL subsection: " +
			subSection->name()};

	RLUnitProcessor rlUnitProcessor {unitNumber};
	rlUnitProcessor.processSection (subSection);

	// Add the unit configuration to the RL device configuration
	rlConfigPtr->rlUnitConfig[unitNumber] = rlUnitProcessor.getConfig ();
}

void RLProcessor::checkConsistency ()
{}

RLConfig RLProcessor::getConfig ()
{
	return *rlConfigPtr;
}
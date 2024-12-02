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

// This is overridden version of SectionProcessor::processSection() with the
// following differences:
// - After a key is processed it is removed from the section,
// - No error is reported on unknown keys/
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
		valueIterator != section->valuesEnd (); ++valueIterator)
	{
		processValue (valueIterator);
		section->removeValue (valueIterator->first);

	}
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
	if (value.asString () == "RL11")
		rlConfigPtr->rlType = RLConfig::RLType::RL11;
	else if (value.asString () == "RLV11")
		rlConfigPtr->rlType = RLConfig::RLType::RLV11;
	else if (value.asString () == "RLV12")
		rlConfigPtr->rlType = RLConfig::RLType::RLV12;
	else
		throw std::invalid_argument {"Incorrect RL controller type: " +
			value.asString ()};
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

RLConfig RLProcessor::getConfig ()
{
	return *rlConfigPtr;
}
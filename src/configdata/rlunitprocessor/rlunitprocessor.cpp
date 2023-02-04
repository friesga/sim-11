#include "rlunitprocessor.h"
#include "../rlunitconfig/rlunitconfig.h"

#include <utility>

using std::make_unique;
using std::move;

RLUnitProcessor::RLUnitProcessor ()
{
	rlUnitConfigPtr = make_unique<RLUnitConfig> ();
}

void RLUnitProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    // Throw exception for non-existing key?
    Process processFunction = valueProcessors[valueIterator->first];
    (this->*processFunction)(valueIterator->second);
}

void RLUnitProcessor::processType (iniparser::Value value)
{
	if (value.asString() == "RL01")
		rlUnitConfigPtr->rlUnitType = RLUnitConfig::RLUnitType::RL01;
	else if (value.asString() == "RL02")
		rlUnitConfigPtr->rlUnitType = RLUnitConfig::RLUnitType::RL02;
	else if (value.asString() == "Auto")
		rlUnitConfigPtr->rlUnitType = RLUnitConfig::RLUnitType::Auto;
	else
		throw std::invalid_argument {"Incorrect RL unit type: " + 
			value.asString()};
}

#ifdef _WIN32
static const string systemType {"Windows"};
#else
static const string systemType {"Linux"};
#endif

// Get filename as a string from the given Value.
// The file name has to be specified as a system-independent string or
// as a map in the format "Windows:filename, Linux:filename".
void RLUnitProcessor::processFileName (iniparser::Value value)
{
	iniparser::Map aMap = value.asMap ();

	// If no system-specific filename is found in the given value map,
    // the unqualified name is returned
	rlUnitConfigPtr->fileName = 
		aMap.getValue (iniparser::Value(systemType),
        aMap.getValue (iniparser::Value(""))).asString();
}

void RLUnitProcessor::processNewFile (iniparser::Value value)
{
	rlUnitConfigPtr->newFile = value.asBool();
}

void RLUnitProcessor::processReadOnly (iniparser::Value value)
{
	rlUnitConfigPtr->readOnly = value.asBool();
}

void RLUnitProcessor::processOverwrite (iniparser::Value value)
{
	rlUnitConfigPtr->overwrite = value.asBool();
}

// Check the consistency of the configuration of a RL unit. Currently
// this is a void.
void RLUnitProcessor::checkConsistency ()
{
}

void RLUnitProcessor::processSubsection (iniparser::Section *subSection)
{}

unique_ptr<DeviceConfig> RLUnitProcessor::getConfig ()
{
	return move (rlUnitConfigPtr);
}
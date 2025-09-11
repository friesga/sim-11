#include "rlunitprocessor.h"
#include "configdata/rl/rlunitconfig/rlunitconfig.h"
#include "configdata/cabinetprocessor/cabinetprocessor.h"

#include <utility>

using std::make_unique;
using std::move;
using std::invalid_argument;

RLUnitProcessor::RLUnitProcessor (size_t unitNumber)
{
	rlUnitConfig.unitNumber = unitNumber;
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
		rlUnitConfig.rlUnitType = RLUnitConfig::RLUnitType::RL01;
	else if (value.asString() == "RL02")
		rlUnitConfig.rlUnitType = RLUnitConfig::RLUnitType::RL02;
	else if (value.asString() == "Auto")
		rlUnitConfig.rlUnitType = RLUnitConfig::RLUnitType::Auto;
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
	rlUnitConfig.fileName = 
		aMap.getValue (iniparser::Value(systemType),
        aMap.getValue (iniparser::Value(""))).asString();
}

void RLUnitProcessor::processNewFile (iniparser::Value value)
{
	rlUnitConfig.newFile = value.asBool();
}

void RLUnitProcessor::processWriteProtect (iniparser::Value value)
{
	rlUnitConfig.writeProtect = value.asBool();
}

void RLUnitProcessor::processOverwrite (iniparser::Value value)
{
	rlUnitConfig.overwrite = value.asBool();
}

void RLUnitProcessor::processCabinet (iniparser::Value value)
{
	rlUnitConfig.cabinetPosition =
		CabinetProcessor::processCabinetKey (value);
}

void RLUnitProcessor::processSpinUpTime (iniparser::Value value)
{
	rlUnitConfig.spinUpTime = value.asInt();
}

// Check the consistency of the configuration of a RL unit.  A valid cabinet
// position has to be specified.
void RLUnitProcessor::checkConsistency ()
{
	if (!rlUnitConfig.cabinetPosition.has_value ())
		throw invalid_argument {"Cabinet position not specified in RL unit section"};
}

void RLUnitProcessor::processSubsection (iniparser::Section *subSection)
{}

RLUnitConfig RLUnitProcessor::getConfig ()
{
	return rlUnitConfig;
}
#include "rk05processor.h"
#include "configdata/rk/rk05/rk05config/rk05config.h"
#include "configdata/cabinetprocessor/cabinetprocessor.h"

#include <utility>

using std::make_unique;
using std::move;
using std::invalid_argument;

RK05Processor::RK05Processor (size_t unitNumber)
{
	rk05ConfigPtr = make_unique<RK05Config> ();
	rk05ConfigPtr->unitNumber = unitNumber;
}

void RK05Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	// Throw exception for non-existing key?
	Process processFunction = valueProcessors[valueIterator->first];
	(this->*processFunction)(valueIterator->second);
}

#ifdef _WIN32
static const string systemType {"Windows"};
#else
static const string systemType {"Linux"};
#endif

// Get filename as a string from the given Value.
// The file name has to be specified as a system-independent string or
// as a map in the format "Windows:filename, Linux:filename".
void RK05Processor::processFileName (iniparser::Value value)
{
	iniparser::Map aMap = value.asMap ();

	// If no system-specific filename is found in the given value map,
	// the unqualified name is returned
	rk05ConfigPtr->fileName =
		aMap.getValue (iniparser::Value (systemType),
			aMap.getValue (iniparser::Value (""))).asString ();
}

void RK05Processor::processNewFile (iniparser::Value value)
{
	rk05ConfigPtr->newFile = value.asBool ();
}

void RK05Processor::processWriteProtect (iniparser::Value value)
{
	rk05ConfigPtr->writeProtect = value.asBool ();
}

void RK05Processor::processOverwrite (iniparser::Value value)
{
	rk05ConfigPtr->overwrite = value.asBool ();
}

void RK05Processor::processCabinet (iniparser::Value value)
{
	rk05ConfigPtr->cabinetPosition =
		CabinetProcessor::processCabinetKey (value);
}

void RK05Processor::processSpinUpTime (iniparser::Value value)
{
	rk05ConfigPtr->spinUpTime = value.asInt ();
}

// Check the consistency of the configuration of a RL unit.  A valid cabinet
// position has to be specified.
void RK05Processor::checkConsistency ()
{
	if (rk05ConfigPtr->cabinetPosition == nullptr)
		throw invalid_argument {"Cabinet position not specified in RK05 section"};
}

void RK05Processor::processSubsection (iniparser::Section* subSection)
{}

shared_ptr<RK05Config> RK05Processor::getConfig ()
{
	return move (rk05ConfigPtr);
}
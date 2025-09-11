#include "rxv21unitprocessor.h"
#include "configdata/rxv21/rxv21unitconfig/rxv21unitconfig.h"

#include <utility>
#include <memory>

using std::make_shared;
using std::move;

void RXV21UnitProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
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
void RXV21UnitProcessor::processFileName (iniparser::Value value)
{
	iniparser::Map aMap = value.asMap ();

	// If no system-specific filename is found in the given value map,
    // the unqualified name is returned
	rxv21UnitConfig.fileName = 
		aMap.getValue (iniparser::Value(systemType),
        aMap.getValue (iniparser::Value(""))).asString();
}

void RXV21UnitProcessor::processNewFile (iniparser::Value value)
{
	rxv21UnitConfig.newFile = value.asBool();
}

void RXV21UnitProcessor::processReadOnly (iniparser::Value value)
{
	rxv21UnitConfig.readOnly = value.asBool();
}

void RXV21UnitProcessor::processOverwrite (iniparser::Value value)
{
	rxv21UnitConfig.overwrite = value.asBool();
}

// Check the consistency of the configuration of a RL unit. Currently
// this is a void.
void RXV21UnitProcessor::checkConsistency ()
{
}

void RXV21UnitProcessor::processSubsection (iniparser::Section *subSection)
{}

shared_ptr<RXV21UnitConfig> RXV21UnitProcessor::getConfig ()
{
	return make_shared<RXV21UnitConfig> (rxv21UnitConfig);
}
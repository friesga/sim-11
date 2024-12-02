#include "ms11pprocessor.h"
#include "touint.h"

#include <utility>

using std::make_unique;
using std::move;
using std::invalid_argument;

MS11PProcessor::MS11PProcessor ()
{
	ms11pConfigPtr = make_unique<MS11PConfig> ();
}

void MS11PProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
		throw std::out_of_range ("Unknown key");

	(this->*processFunction)(valueIterator->second);
}

void MS11PProcessor::processPowerSource (iniparser::Value value)
{
	map<string, MS11PConfig::PowerSource>::iterator iter;

	if ((iter = validPowerSources.find (value.asString ())) !=
		validPowerSources.end ())
		ms11pConfigPtr->powerSource = iter->second;
	else
		throw invalid_argument {"Incorrect MS11-P power_source value"};
}

void MS11PProcessor::processStartingAddress (iniparser::Value value)
{
	try
	{
		ms11pConfigPtr->startingAddress = touint<u32> (value.asString ());
	}
	catch (std::invalid_argument const&)
	{
		throw invalid_argument {"Incorrect starting address in MS11-P section specified: " +
			value.asString ()};
	}
}

void MS11PProcessor::processCsrAddress (iniparser::Value value)
{
	try
	{
		ms11pConfigPtr->csrAddress = touint<u32> (value.asString ());
	}
	catch (invalid_argument const&)
	{
		throw invalid_argument {"Incorrect CSR address in MS11-P section specified: " +
			value.asString ()};
	}
}

// Check the consistency of the configuration of the MSV11 memory.
void MS11PProcessor::checkConsistency ()
{
	if (ms11pConfigPtr->startingAddress > 016000000)
		throw invalid_argument {"MS11-P maximum starting address is 016000000"};

	if (ms11pConfigPtr->startingAddress % 040000 != 0)
		throw invalid_argument {"MS11-P address must start at 8KW boundary"};
}

void MS11PProcessor::processSubsection (iniparser::Section* subSection)
{
}

DeviceConfig MS11PProcessor::getConfig ()
{
	return move (ms11pConfigPtr);
}
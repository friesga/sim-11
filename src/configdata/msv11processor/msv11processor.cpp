#include "msv11processor.h"

#include <utility>

using std::make_unique;
using std::move;
using std::invalid_argument;

MSV11Processor::MSV11Processor ()
{
	msv11ConfigPtr = make_unique<MSV11Config> ();
}

void MSV11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
		throw std::out_of_range ("Unknown key");

    (this->*processFunction)(valueIterator->second);
}

void MSV11Processor::processPowerSource (iniparser::Value value)
{
	map<string, MSV11Config::PowerSource>::iterator iter;

    if ((iter = validPowerSources.find (value.asString ())) != 
            validPowerSources.end ())
        msv11ConfigPtr->powerSource = iter->second;
    else
        throw invalid_argument {"Incorrect MSV11 power_source value"};
}

void MSV11Processor::processStartingAddress (iniparser::Value value)
{
	try
	{
		msv11ConfigPtr->startingAddress = touint<u32> (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw invalid_argument {"Incorrect address in MSV11 section specified: " + 
			value.asString()};
	}
}

void MSV11Processor::processBank7Lower2kW (iniparser::Value value)
{
	try
	{
		msv11ConfigPtr->bank7Lower2kWEnabled = value.asBool ();
	}
	catch (invalid_argument const &)
	{
		throw invalid_argument {"Value of bank7_lower2K_enabled must be \'true\' or \'false\'"};
	}
}

// Check the consistency of the configuration of the MSV11 memory.
void MSV11Processor::checkConsistency ()
{
	if (msv11ConfigPtr->startingAddress > 0760000)
		throw invalid_argument {"MSV11 maximum starting address is 0760000"};

	if (msv11ConfigPtr->startingAddress % 020000 != 0)
		throw invalid_argument {"MSV11 address must start at 4K boundary"};
}

void MSV11Processor::processSubsection (iniparser::Section *subSection)
{}

DeviceConfigVariant MSV11Processor::getConfig ()
{
	return move (msv11ConfigPtr);
}
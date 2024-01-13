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
    // Throw exception for non-existing key?
    Process processFunction = valueProcessors[valueIterator->first];
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

// Check the consistency of the configuration of the MSV11 memory. Currently
// this is a void.
void MSV11Processor::checkConsistency ()
{
}

void MSV11Processor::processSubsection (iniparser::Section *subSection)
{}

unique_ptr<DeviceConfig> MSV11Processor::getConfig ()
{
	return move (msv11ConfigPtr);
}
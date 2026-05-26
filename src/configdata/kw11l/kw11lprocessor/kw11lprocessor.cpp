#include "kw11lprocessor.h"

#include <utility>

using std::out_of_range;

void KW11LProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
		throw std::out_of_range ("Unknown key");

	(this->*processFunction)(valueIterator->second);
}

void KW11LProcessor::processClockSource (iniparser::Value value)
{
	try
	{
		kw11lConfig_.clockSource = clockSourceSpec.at (value.asString ());
	}
	catch (out_of_range const&)
	{
		throw invalid_argument {"Incorrect KW11-L clock source: " +
			value.asString ()};
	}
}

DeviceConfig KW11LProcessor::getConfig ()
{
    return kw11lConfig_;
}
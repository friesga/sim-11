#include "sluprocessor.h"
#include "../sluconfig/sluconfig.h"

#include <utility>
#include <string>
#include <algorithm>

using std::make_unique;
using std::move;
using std::invalid_argument;
using std::move;
using std::string;
using std::ranges::find_if;
using std::vector;
using std::pair;

SLUProcessor::SLUProcessor ()
{
	sluConfigPtr = make_unique<SLUConfig> ();
}

void SLUProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    // Throw exception for non-existing key?
    Process processFunction = valueProcessors[valueIterator->first];
    (this->*processFunction)(valueIterator->second);
}

void SLUProcessor::checkConsistency ()
{}

void SLUProcessor::processSubsection (iniparser::Section *subSection)
{}

void SLUProcessor::processSLU1Enabled (iniparser::Value value)
{
    processSLUxEnabled (0, "slu1", value);
}

void SLUProcessor::processSLU2Enabled (iniparser::Value value)
{
    processSLUxEnabled (1, "slu2", value);
}

void SLUProcessor::processSLUxEnabled (size_t unitNr, string id, iniparser::Value value)
{
    try
	{
		sluConfigPtr->uartConfig[unitNr].enabled_ = value.asBool ();
	}
	catch (invalid_argument const &)
	{
		throw invalid_argument {"Value of " + id + "_enabled must be \'true\' or \'false\'"};
	}
}

void SLUProcessor::processSLU2Address (iniparser::Value value)
{
	vector<pair<u16, u16>>::iterator it;

	try
	{
		u16 address = touint<u16> (value.asString());
		it = find_if (sluConfigPtr->validSlu2AddressVector,
			[&address] (pair<u16, u16> element) {return element.first == address;});
	}
	catch (std::invalid_argument const &)
	{
		throw invalid_argument {"Incorrect SLU2 address in DLV11-J section specified: " + 
			value.asString()};
	}

	if (it == sluConfigPtr->validSlu2AddressVector.end ())
		throw invalid_argument {"KDF11-B SLU2 address must be either 0176500 or 0176540"}; 
		
	sluConfigPtr->uartConfig[1].baseAddress_ = it->first;
	sluConfigPtr->uartConfig[1].baseVector_ = it->second;
}

void SLUProcessor::processSLU2Vector (iniparser::Value value)
{}

void SLUProcessor::processBreakResponse (iniparser::Value value)
{}

void SLUProcessor::processBreakKey (iniparser::Value value)
{}

void SLUProcessor::processLoopback (iniparser::Value value)
{}

unique_ptr<DeviceConfig> SLUProcessor::getConfig ()
{
	return move (sluConfigPtr);
}
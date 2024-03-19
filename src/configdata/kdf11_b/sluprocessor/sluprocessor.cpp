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

void SLUProcessor::processBreakResponse (iniparser::Value value)
{
	map<string, ConsoleConfig::BreakResponse>::iterator iter;

    if ((iter = validBreakResponses.find (value.asString ())) != 
            validBreakResponses.end ())
        sluConfigPtr->consoleConfig.breakResponse = iter->second;
    else
        throw invalid_argument {"Incorrect KDF11-B SLU break_response value"};
}

void SLUProcessor::processBreakKey (iniparser::Value value)
{
	if (value.asString () == "esc")
        sluConfigPtr->consoleConfig.breakKey = 27;
    else if (value.asString().starts_with ('^') &&
            value.asString ().size () == 2)
        sluConfigPtr->consoleConfig.breakKey = value.asString ().at (1) & ~11100000;
    else
            throw invalid_argument {"Incorrect break key specification"};
}

void SLUProcessor::processLoopback (iniparser::Value value)
{
	bool loopback;

	try
	{
		loopback = value.asBool ();
	}
	catch (invalid_argument const &)
	{
		throw invalid_argument {"Value of KDF11-B SLU loopback must be \'true\' or \'false\'"};
	}

	sluConfigPtr->uartConfig[0].loopback_ = loopback;
	sluConfigPtr->uartConfig[1].loopback_ = loopback;
}

DeviceConfigVariant SLUProcessor::getConfig ()
{
	return move (sluConfigPtr);
}
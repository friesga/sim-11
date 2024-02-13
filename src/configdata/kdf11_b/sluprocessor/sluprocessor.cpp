#include "sluprocessor.h"
#include "../sluconfig/sluconfig.h"

#include <utility>

using std::make_unique;
using std::move;
using std::invalid_argument;
using std::move;

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
    try
	{
		sluConfigPtr->uartConfig[0].enabled_ = value.asBool ();
	}
	catch (invalid_argument const &)
	{
		throw invalid_argument {"Value of slu1_enabled must be \'true\' or \'false\'"};
	}
}

void SLUProcessor::processSLU2Enabled (iniparser::Value value)
{
    try
	{
		sluConfigPtr->uartConfig[1].enabled_ = value.asBool ();
	}
	catch (invalid_argument const &)
	{
		throw invalid_argument {"Value of slu2_enabled must be \'true\' or \'false\'"};
	}
}

void SLUProcessor::processSLU2Address (iniparser::Value value)
{
	u16 address;

	try
	{
		address = touint<u16> (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw invalid_argument {"Incorrect SLU2 address in DLV11-J section specified: " + 
			value.asString()};
	}

	if (address == 0176500)
	{
		sluConfigPtr->uartConfig[1].baseAddress_ = address;
		sluConfigPtr->uartConfig[1].baseVector_ = 0300;
	}
	else if (address == 0176540)
	{
		sluConfigPtr->uartConfig[1].baseAddress_ = address;
		sluConfigPtr->uartConfig[1].baseVector_ = 0340;
	}
	else
		throw invalid_argument {"SLU2 address must be either 0176500 or 0176540"}; 

	sluConfigPtr->uartConfig[1].baseAddress_ = address;
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
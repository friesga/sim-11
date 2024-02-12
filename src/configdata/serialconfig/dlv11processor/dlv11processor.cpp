#include "dlv11processor.h"
#include "configdata/serialconfig/dlv11jconfig/dlv11jconfig.h"

#include <utility>

using std::make_unique;
using std::move;
using std::invalid_argument;

DLV11Processor::DLV11Processor ()
{
	dlConfigPtr = make_unique<DLV11JConfig> ();
}

void DLV11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    // Throw exception for non-existing key?
    Process processFunction = valueProcessors[valueIterator->first];
    (this->*processFunction)(valueIterator->second);
}

void DLV11Processor::processAddress (iniparser::Value value)
{
	try
	{
		dlConfigPtr->baseAddress = touint<u16> (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw invalid_argument {"Incorrect address in DLV11-J section specified: " + 
			value.asString()};
	}
}

void DLV11Processor::processVector (iniparser::Value value) 
{ 
	try
	{
		dlConfigPtr->baseVector = touint<u16> (value.asString());
	}
	catch (invalid_argument const &)
	{
		throw invalid_argument {"Incorrect vector in DLV11-J section specified: " + 
			value.asString()};
	}
}

void DLV11Processor::processConsoleEnabled (iniparser::Value value)
{
	try
	{
		dlConfigPtr->ch3ConsoleEnabled = value.asBool ();
	}
	catch (invalid_argument const &)
	{
		throw invalid_argument {"Value of ch3_console_enabled must be \'true\' or \'false\'"};
	}
}

// Find the specified value for the channel 3 break response in the valid
// values.
void DLV11Processor::processBreakResponse (iniparser::Value value)
{
    map<string, DLV11JConfig::BreakResponse>::iterator iter;

    if ((iter = validBreakResponses.find (value.asString ())) != 
            validBreakResponses.end ())
        dlConfigPtr->breakResponse = iter->second;
    else
        throw invalid_argument {"Incorrect ch3_break_response value"};
}

void DLV11Processor::processBreakKey (iniparser::Value value)
{
    // dlConfigPtr->breakKey = static_cast<unsigned char> (value.asInt ());
    if (value.asString () == "esc")
        dlConfigPtr->breakKey = 27;
    else if (value.asString().starts_with ('^') &&
            value.asString ().size () == 2)
        dlConfigPtr->breakKey = value.asString ().at (1) & ~11100000;
    else
            throw invalid_argument {"Incorrect break key specification"};
}

void DLV11Processor::processLoopback (iniparser::Value value)
{
	try
	{
		dlConfigPtr->loopback = value.asBool ();
	}
	catch (invalid_argument const &)
	{
		throw invalid_argument {"Value of loopback must be \'true\' or \'false\'"};
	}
}

// Check the consistency of the configuration of the DLV11 controller.
void DLV11Processor::checkConsistency ()
{
	checkBaseAddress ();

	// Now we determined the configured base address of the DLV11-J is valid
	// we can use it to create the configuration for the channels.
	dlConfigPtr->createDLV11J_UARTsConfig ();
}

void DLV11Processor::checkBaseAddress ()
{
		// If a base address is specified, check it is in the valid range
	if (dlConfigPtr->baseAddress > 0)
	{
		if (dlConfigPtr->baseAddress < 0160000 ||
			dlConfigPtr->baseAddress > 0177770)
			throw invalid_argument {"DLV11-J base address must be in range 0160000 - 0177770"};
	}

	// if channel 3 is to function as the console device, the base address
	// must be configured for one of three addresses: 176500 (factory
	// configuration), 176540 or 177500 (EK-DLV1J-UG-001). 
	if (dlConfigPtr->ch3ConsoleEnabled  &&
		!(dlConfigPtr->baseAddress == 0 || 
		  dlConfigPtr->baseAddress == 0176500 ||
		  dlConfigPtr->baseAddress == 0176540 ||
		  dlConfigPtr->baseAddress == 0177500))
	{
		throw invalid_argument {"DLV11-J base address must be 0176500, 0176540 or 0177500"};
	}
}

void DLV11Processor::processSubsection (iniparser::Section *subSection)
{}

unique_ptr<DeviceConfig> DLV11Processor::getConfig ()
{
	return move (dlConfigPtr);
}
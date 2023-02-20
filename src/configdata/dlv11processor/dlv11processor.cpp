#include "dlv11processor.h"
#include "../dlv11config/dlv11config.h"

#include <utility>

using std::make_unique;
using std::move;

DLV11Processor::DLV11Processor ()
{
	dlv11ConfigPtr = make_unique<DLV11Config> ();
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
		dlv11ConfigPtr->baseAddress = touint16_t (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"Incorrect address in DLV11-J section specified: " + 
			value.asString()};
	}
}

void DLV11Processor::processVector (iniparser::Value value) 
{ 
	try
	{
		dlv11ConfigPtr->vector = touint16_t (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw std::invalid_argument {"Incorrect vector in DLV11-J section specified: " + 
			value.asString()};
	}
}

// Find the specified value for the channel 3 break response in the valid
// values.
void DLV11Processor::processBreakResponse (iniparser::Value value)
{
    map<string, DLV11Config::Ch3BreakResponse>::iterator iter;

    if ((iter = validBreakResponses.find (value.asString ())) != 
            validBreakResponses.end ())
        dlv11ConfigPtr->ch3BreakResponse = iter->second;
    else
        throw std::invalid_argument {"Incorrect ch3_break_response value"};
}

void DLV11Processor::processBreakKey (iniparser::Value value)
{
    // dlv11ConfigPtr->breakKey = static_cast<unsigned char> (value.asInt ());
    if (value.asString () == "esc")
        dlv11ConfigPtr->breakKey = 27;
    else if (value.asString().starts_with ('^') &&
            value.asString ().size () == 2)
        dlv11ConfigPtr->breakKey = value.asString ().at (1) & ~11100000;
    else
            throw std::invalid_argument {"Incorrect break key specification"};
}

// Check the consistency of the configuration of the DLV11 controller. Currently
// this is a void.
void DLV11Processor::checkConsistency ()
{
}

void DLV11Processor::processSubsection (iniparser::Section *subSection)
{}

unique_ptr<DeviceConfig> DLV11Processor::getConfig ()
{
	return move (dlv11ConfigPtr);
}
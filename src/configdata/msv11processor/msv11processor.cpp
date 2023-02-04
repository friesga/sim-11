#include "msv11processor.h"

#include <utility>

using std::make_unique;
using std::move;

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
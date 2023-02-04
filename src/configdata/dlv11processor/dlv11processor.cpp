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
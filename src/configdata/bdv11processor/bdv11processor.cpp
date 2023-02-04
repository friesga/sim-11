#include "bdv11processor.h"

#include <utility>

using std::make_unique;
using std::move;

BDV11Processor::BDV11Processor ()
{
	bdv11ConfigPtr = make_unique<BDV11Config> ();
}

void BDV11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    // Throw exception for non-existing key?
    Process processFunction = valueProcessors[valueIterator->first];
    (this->*processFunction)(valueIterator->second);
}

// Check the consistency of the configuration of the BDV11 controller. Currently
// this is a void.
void BDV11Processor::checkConsistency ()
{
}

void BDV11Processor::processSubsection (iniparser::Section *subSection)
{}

unique_ptr<DeviceConfig> BDV11Processor::getConfig ()
{
	return move (bdv11ConfigPtr);
}
#include "kdv11processor.h"
#include "../kdv11config/kdv11config.h"

using std::make_unique;
using std::move;

KDV11Processor::KDV11Processor ()
{
	kdv11ConfigPtr = make_unique<KDV11Config> ();
}

void KDV11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{}

// Check the consistency of the configuration of the KDV11. Currently
// this is a void.
void KDV11Processor::checkConsistency ()
{
}

void KDV11Processor::processSubsection (iniparser::Section *subSection)
{}

unique_ptr<DeviceConfig> KDV11Processor::getConfig ()
{
	return move (kdv11ConfigPtr);
}
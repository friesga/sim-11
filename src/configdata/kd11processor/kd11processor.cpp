#include "kd11processor.h"
#include "../kd11config/kd11config.h"

using std::make_unique;
using std::move;

KD11Processor::KD11Processor ()
{
	kd11ConfigPtr = make_unique<KD11Config> ();
}

void KD11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{}

// Check the consistency of the configuration of the KD11. Currently
// this is a void.
void KD11Processor::checkConsistency ()
{
}

void KD11Processor::processSubsection (iniparser::Section *subSection)
{}

unique_ptr<DeviceConfig> KD11Processor::getConfig ()
{
	return move (kd11ConfigPtr);
}
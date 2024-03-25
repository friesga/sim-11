#include "kd11_naprocessor.h"
#include "../kd11_naconfig/kd11_naconfig.h"

using std::move;
using std::invalid_argument;

KD11_NAProcessor::KD11_NAProcessor ()
{}

void KD11_NAProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
        // This exception will be catched and processed in 
        // SectionProcessor::processSection().
		throw std::out_of_range ("Unknown key in KD11-NA section");

    (this->*processFunction)(valueIterator->second);
}

void KD11_NAProcessor::checkConsistency ()
{}

void KD11_NAProcessor::processSubsection (iniparser::Section *subSection)
{}

void KD11_NAProcessor::processPowerUpMode (iniparser::Value value)
{
	KD11Processor kd11Procesor;
	kd11_naConfigPtr->powerUpMode = 
		kd11Procesor.processPowerUpMode (value);
}

DeviceConfigVariant KD11_NAProcessor::getConfig ()
{
	return move (kd11_naConfigPtr);
}
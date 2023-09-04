#include "kd11_naprocessor.h"
#include "../kd11_naconfig/kd11_naconfig.h"

using std::make_unique;
using std::move;
using std::invalid_argument;

KD11_NAProcessor::KD11_NAProcessor ()
{
	kd11ConfigPtr = make_unique<KD11_NAConfig> ();
}

void KD11_NAProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];
    (this->*processFunction)(valueIterator->second);
}

void KD11_NAProcessor::processPowerUpMode (iniparser::Value value)
{
	map<string, KD11_NAConfig::PowerUpMode>::iterator iter;

    if ((iter = validPowerUpModes.find (value.asString ())) != 
            validPowerUpModes.end ())
        kd11ConfigPtr->powerUpMode = iter->second;
    else
        throw invalid_argument {"Incorrect KD11-NA power-up_mode value"};
}

// Check the consistency of the configuration of the KD11-NA. Currently
// this is a void.
void KD11_NAProcessor::checkConsistency ()
{
}

void KD11_NAProcessor::processSubsection (iniparser::Section *subSection)
{}

unique_ptr<DeviceConfig> KD11_NAProcessor::getConfig ()
{
	return move (kd11ConfigPtr);
}
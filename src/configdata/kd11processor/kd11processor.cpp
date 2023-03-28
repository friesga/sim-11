#include "kd11processor.h"
#include "../kd11config/kd11config.h"

using std::make_unique;
using std::move;
using std::invalid_argument;

KD11Processor::KD11Processor ()
{
	kd11ConfigPtr = make_unique<KD11Config> ();
}

void KD11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];
    (this->*processFunction)(valueIterator->second);
}

void KD11Processor::processPowerUpMode (iniparser::Value value)
{
	map<string, KD11Config::PowerUpMode>::iterator iter;

    if ((iter = validPowerUpModes.find (value.asString ())) != 
            validPowerUpModes.end ())
        kd11ConfigPtr->powerUpMode = iter->second;
    else
        throw invalid_argument {"Incorrect KD11 power-up_mode value"};
}

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
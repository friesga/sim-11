#include "kd11processor.h"

using std::make_unique;
using std::move;
using std::invalid_argument;

KD11Processor::KD11Processor ()
{}

void KD11Processor::init (KD11Config* kd11Config)
{
    kd11Config_ = kd11Config;
}

void KD11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

    if (processFunction == nullptr)
        throw invalid_argument {"Unknown KD11 option: " + valueIterator->first};

    (this->*processFunction)(valueIterator->second);
}

void KD11Processor::processPowerUpMode (iniparser::Value value)
{
	map<string, KD11Config::PowerUpMode>::iterator iter;

    if ((iter = validPowerUpModes.find (value.asString ())) != 
            validPowerUpModes.end ())
        kd11Config_->powerUpMode = iter->second;
    else
        throw invalid_argument {"Incorrect KD11 power-up_mode value"};
}

// Check the consistency of the configuration of the KD11-NA. Currently
// this is a void.
void KD11Processor::checkConsistency ()
{}

void KD11Processor::processSubsection (iniparser::Section *subSection)
{}

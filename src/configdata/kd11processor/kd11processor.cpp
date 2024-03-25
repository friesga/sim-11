#include "kd11processor.h"

using std::invalid_argument;

KD11Config::PowerUpMode KD11Processor::processPowerUpMode (iniparser::Value value)
{
	map<string, KD11Config::PowerUpMode>::iterator iter;

    if ((iter = validPowerUpModes.find (value.asString ())) != 
            validPowerUpModes.end ())
        return iter->second;
    else
        throw invalid_argument {"Incorrect KD11 power-up_mode value"};
}

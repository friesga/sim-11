#ifndef _KD11PROCESSOR_H_
#define _KD11PROCESSOR_H_

#include "configdata/kd11config/kd11config.h"
#include "configdata/sectionprocessor/sectionprocessor.h"

#include <map>
#include <string>

using std::map;
using std::string;

class KD11Processor
{
public:
	KD11Config::PowerUpMode processPowerUpMode (iniparser::Value value);

private:
	map<string, KD11Config::PowerUpMode> validPowerUpModes =
	{
		{"vector",    KD11Config::PowerUpMode::Vector},
		{"ODT",       KD11Config::PowerUpMode::ODT},
		{"bootstrap", KD11Config::PowerUpMode::Bootstrap},
	};
};

#endif // _KD11PROCESSOR_H_
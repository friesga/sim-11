#ifndef _RLUNITCONFIG_H_
#define _RLUNITCONFIG_H_

#include "cabinet/cabinet.h"

#include <string>
#include <memory>

using std::string;
using std::shared_ptr;

struct RLUnitConfigParams;

// Definition of the configuration result of an RL01/02 unit.
struct RLUnitConfig
{
    enum class RLUnitType
	{
		RL01,
		RL02,
		Auto
	};

	size_t unitNumber {0};
	RLUnitType rlUnitType {RLUnitType::RL01};
	shared_ptr<Cabinet::Position> cabinetPosition {nullptr};
	string fileName {};
	bool newFile {false};
	bool writeProtect {false};
	bool overwrite {false};
	size_t spinUpTime {0};

    RLUnitConfig ();
	RLUnitConfig (RLUnitConfigParams const &rlUnitConfigParams);

	// The RL01/02 has a height of six rack units
	static const RackUnit unitHeight;
};

// The struct RLUnitConfigParams and the constructor 
// RLUnitConfig (RLUnitConfigParams const &rlUnitConfigParams) are used to
// initialize RLUnitConfig object in the unit tests by means of a designated
// initializer list. The RLUnitConfig struct itself cannot be initialized
// that way as it has a constructor.
struct RLUnitConfigParams
{
	string fileName {};
	bool newFile {false};
	bool readOnly {false};
	bool overwrite {false};
	bool writeProtect {false};
	RLUnitConfig::RLUnitType unitType {RLUnitConfig::RLUnitType::RL01};
};

#endif // _RLUNITCONFIG_H_
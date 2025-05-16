#ifndef _RK05CONFIG_H_
#define _RK05CONFIG_H_

#include "cabinet/cabinet.h"
#include "types.h"

#include <memory>

using std::shared_ptr;

struct RK05ConfigParams;

// Definition of the configuration options of an RK05 unit
struct RK05Config
{
	u16 unitNumber {0};
	shared_ptr<Cabinet::Position> cabinetPosition {nullptr};
	string fileName {};
	bool newFile {false};
	bool writeProtect {false};
	bool overwrite {false};
	size_t spinUpTime {0};

	RK05Config ();
	RK05Config (RK05ConfigParams const& rk05ConfigParams);

	// The RK05 has a height of six rack units
	static const RackUnit unitHeight;
};

// The struct RK05ConfigParams and the constructor 
// RK05Config (RK05ConfigParams const &rk05ConfigParams) are used to
// initialize RK05Config objects in the unit tests by means of a designated
// initializer list. The RK05Config struct itself cannot be initialized
// that way as it has a constructor.
struct RK05ConfigParams
{
	string fileName {};
	bool newFile {false};
	bool readOnly {false};
	bool overwrite {false};
	bool writeProtect {false};
};

#endif // _RK05CONFIG_H_
#ifndef _RK11DCONFIG_H_
#define _RK11DCONFIG_H_

#include "types.h"
#include "devicetype.h"
#include "configdata/rk/rk05/rk05config/rk05config.h"

#include <memory>

using std::shared_ptr;

// Definition of the configuration of a RK11-D controller
//
// The configuration contains:
// - The registers base address (760000 - 777770, default 0777400),
// - The device vector (0-774, default 0220),
// - The bus request level (BR4, BR5, BR6 or BR7, default BR5),
// - The number of RK05 drives attached to this controller,
// - References to the attached drives.
//
// A RK11-D also contains "seven inhibit strobe jumpers on the M7257 module
// which, when cut, inhibit the loading of a particular programmable
// register". This feature is currently not implemented.
//
struct RK11DConfig : public DeviceType<BusType::Unibus>
{
	static constexpr size_t maxRK05Units = 8;
	u16 address {0177400};
	u16 vector {0220};
	u8 busRequestLevel {5};
	size_t numUnits {0};

	shared_ptr<RK05Config> rk05Config[maxRK05Units] = {};
};

#endif // _RK11DCONFIG_H_
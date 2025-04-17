#ifndef _RK11DCONFIG_H_
#define _RK11DCONFIG_H_

#include "types.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/rk/rk05/rk05config/rk05config.h"

#include <memory>

using std::shared_ptr;

// Definition of the configuration of a RK11-D controller
//
struct RK11DConfig : public DeviceType<BusType::Unibus>
{
	static constexpr size_t maxRK05Units = 8;
	u16 address {0};
	u16 vector {0};
	size_t numUnits {0};

	shared_ptr<RK05Config> rk05Config[maxRK05Units] = {};
};

#endif // _RK11DCONFIG_H_
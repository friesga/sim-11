#ifndef _RXCONFIG_H_
#define _RXCONFIG_H_

#include "types.h"
#include "configdata/rxv21unitconfig/rxv21unitconfig.h"

#include <memory>

using std::shared_ptr;

struct RXV21Config
{
    static constexpr size_t numRxUnits = 2;
	u16 address {0};
	u16 vector {0};
	shared_ptr<RXV21UnitConfig> rxv21UnitConfig[numRxUnits] = {};
};

#endif // _RXCONFIG_H_
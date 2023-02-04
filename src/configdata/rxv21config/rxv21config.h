#ifndef _RXCONFIG_H_
#define _RXCONFIG_H_

#include "../deviceconfig/deviceconfig.h"

// ToDo: Replace by inclusion of types.h
#include <cstdint>
#include <cstddef>

#include <memory>

using std::shared_ptr;

struct RXV21Config : public DeviceConfig
{
    static constexpr size_t numRxUnits = 2;
	uint16_t address {0};
	uint16_t vector {0};
	shared_ptr<DeviceConfig> rxv21UnitConfig[numRxUnits] = {};

	RXV21Config ();
};

#endif // _RXCONFIG_H_
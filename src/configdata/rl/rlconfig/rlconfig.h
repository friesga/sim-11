#ifndef _RLCONFIG_H_
#define _RLCONFIG_H_

#include "devicetype.h"
#include "types.h"
#include "configdata/rl/rlunitconfig/rlunitconfig.h"

#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;

// Definition of the configuration of an RL controller.
// 
// Three type of RL controllers exist:
// - RL11. The RL11 controller is used to interface with the Unibus,
// - RLV11. This controller interfaces the drive with the LSI-11 bus,
//   i.e. the 18-bit Qbus,
// - RLV12. It is used to interface the drive with the LSI-11 (18-bit) or
//   Extended LSI-11 (22-bit) Qbus.
// 
// Source: RL01/RL02 User Guide (EK-RL012-UG-005)
//
// Definition of the parameters common to the RL11, RLV11 and RLV12.
//
struct RLConfig
{
	enum class RLType
	{
		RL11,
		RLV11,
		RLV12
	};

	static constexpr size_t maxRlUnits = 4;
	uint16_t address {0};
	uint16_t vector {0};
	size_t numUnits {0};
	RLType rlType {RLType::RLV12};

	shared_ptr<RLUnitConfig> rlUnitConfig[maxRlUnits] = {};
};

// The RLV11 configuration has no parameters other than the common
// configuration parameters.
struct RLV11Config : public DeviceType<BusType::QBus>
{
	RLConfig common;
};

// Configuration parameters specific for the RLV12.
struct RLV12Config : public DeviceType<BusType::QBus>
{
	RLConfig common;
	bool _22bit {false};
};

#endif // _RLCONFIG_H_

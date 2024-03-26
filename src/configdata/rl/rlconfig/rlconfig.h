#ifndef _RLCONFIG_H_
#define _RLCONFIG_H_

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
	bool _22bit {false};

	shared_ptr<RLUnitConfig> rlUnitConfig[maxRlUnits] = {};
};

#endif // _RLCONFIG_H_

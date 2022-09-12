#ifndef _CONFIGDATA_H_
#define _CONFIGDATA_H_

#include <cstdint>
#include <string>

// Definition of the configuration result of an RL01/02 unit.
struct RlUnitConfig
{
	bool RL01 {true};
	std::string fileName;
	bool newFile {false};
	bool readOnly {false};
	bool overwrite {false};
};

// Three type of RL controllers exist:
// - RL11. The RL11 controller is used to interface with the Unibus,
// - RLV11. This controller interfaces the drive with the LSI-11 bus,
//   i.e. the 18-bit Qbus,
// - RLV12. It is used to interface the drive with the LSI-11 (18-bit) or
//   Extended LSI-11 (22-bit) Qbus.
// 
// Source: RL01/RL02 User Guide (EK-RL012-UG-005)
//
struct RlConfig
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
	RlUnitConfig rlUnitConfig[maxRlUnits];
};

//
// Definition of the configuration data for the devices in the simulator.
// 
struct DeviceConfig
{
	RlConfig *rlConfig = nullptr;
};

#endif // !_CONFIGDATA_H_

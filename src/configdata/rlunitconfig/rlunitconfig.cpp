#include "rlunitconfig.h"

RLUnitConfig::RLUnitConfig ()
    : 
    // Currently there is no need for a separate RLUnit device type. The unit
    // type (RL01/RL02) could be administrated in the device type but the
    // unit type is a section attribute and that attribute is processed after
    // the RLUnitConfig object is created.
    DeviceConfig (DeviceType::RLV12)
{}

// Constructor to be able to initialize the RLUnitConfig struct by means of
// designated initializers.
RLUnitConfig::RLUnitConfig (RLUnitConfigParams const &rlUnitConfigParams)
    :
    DeviceConfig (DeviceType::RLV12),
    fileName {rlUnitConfigParams.fileName},
	newFile {rlUnitConfigParams.newFile},
	readOnly {rlUnitConfigParams.readOnly},
	overwrite {rlUnitConfigParams.overwrite}
{}
#include "rlunitconfig.h"

const RackUnit RLUnitConfig::unitHeight {6_ru};

RLUnitConfig::RLUnitConfig ()
{}

// Constructor to be able to initialize the RLUnitConfig struct by means of
// designated initializers.
RLUnitConfig::RLUnitConfig (RLUnitConfigParams const &rlUnitConfigParams)
    :
    fileName {rlUnitConfigParams.fileName},
	newFile {rlUnitConfigParams.newFile},
	writeProtect {rlUnitConfigParams.writeProtect},
	overwrite {rlUnitConfigParams.overwrite}
{}
#include "rlunitconfig.h"

RLUnitConfig::RLUnitConfig ()
{}

// Constructor to be able to initialize the RLUnitConfig struct by means of
// designated initializers.
RLUnitConfig::RLUnitConfig (RLUnitConfigParams const &rlUnitConfigParams)
    :
    fileName {rlUnitConfigParams.fileName},
	newFile {rlUnitConfigParams.newFile},
	readOnly {rlUnitConfigParams.readOnly},
	overwrite {rlUnitConfigParams.overwrite}
{}
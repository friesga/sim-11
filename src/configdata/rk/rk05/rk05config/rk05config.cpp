#include "rk05config.h"

const RackUnit RK05Config::unitHeight {6_ru};

RK05Config::RK05Config ()
{}

// Constructor to be able to initialize the RK05Config struct by means of
// designated initializers.
RK05Config::RK05Config (RK05ConfigParams const& rk05ConfigParams)
	:
	fileName {rk05ConfigParams.fileName},
	newFile {rk05ConfigParams.newFile},
	writeProtect {rk05ConfigParams.writeProtect},
	overwrite {rk05ConfigParams.overwrite}
{}
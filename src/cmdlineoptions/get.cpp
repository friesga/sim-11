#include "cmdlineoptions.h"

CmdLineOptions &CmdLineOptions::get()
{
	static CmdLineOptions cmdLineOptions;
	return cmdLineOptions;
};
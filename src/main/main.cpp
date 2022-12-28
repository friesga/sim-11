#define _POSIX_C_SOURCE 200809L

#include "main.h"
#include "trace/trace.h"
#include "ba11_n/ba11_n.h"
#include "rxv21/rxv21.h"
#include "rlv12/rlv12.h"
#include "bdv11/bdv11.h"
#include "dlv11j/dlv11j.h"
#include "msv11d/msv11d.h"
#include "lsi11/lsi11.h"
#include "console/console.h"
#include "cmdlineoptions/cmdlineoptions.h"
#include "configdata/configprocessor/configprocessor.h"
#include "logger/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>

#include <iostream>
#include <thread>
#include <memory>

using std::shared_ptr;
using std::make_shared;

// Get rid of SDL's main function
#undef main

/* #define DEBUG */

#define	READ(addr)			lsi11_.bus.read(lsi11_.bus.user, (addr))
#define	WRITE(addr, val)	lsi11_.bus.write(lsi11_.bus.user, (addr), (val))

int main (int argc, char const **argv)
try
{
	Main simulator;

	simulator.init (argc, argv);

	// Configure the devices with the parameters specified in the
	// configuration file
	// ToDo: Change CmdLineOptions singleton to a Main class data member
	simulator.configureDevices (CmdLineOptions::get().config_file,
		CmdLineOptions::get().floppy_filename);
	
	if (CmdLineOptions::get().load_file) 
		simulator.loadFile (CmdLineOptions::get().load_file);

	// Run the simulator
	simulator.run();

	printf ("\n");

	if(CmdLineOptions::get().trace_file)
	{
		TRCFINISH();
	}
}
catch (std::string msg)
{
	std::cerr << msg << '\n';
	return 1;
}

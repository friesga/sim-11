#include "cmdlineoptions.h"

#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;

void CmdLineOptions::processOptions (int argc, char const **argv)
{
	stringstream msg;

	// Reset options in case a new set of options has to be processed
	reset ();

	// First argument is the executable's name
	const char* self = *argv;
    --argc;
	++argv;

	for (; argc; --argc, ++argv)
	{
		if (!strcmp("-h", *argv))
			halt_ = true;
		else if(!strcmp("-b", *argv)) 
			bootstrap_ = true;
		else if(!strcmp("-z", *argv)) 
			compress_ = true;
		else if(!strcmp("-x", *argv))
			exit_on_halt_ = true;
		else if(!strcmp("-l", *argv) && argc > 1) 
		{
			load_file_ = argv[1];
			--argc;
			++argv;
		}
		else if(!strcmp("-c", *argv) && argc > 1) 
		{
			config_file_ = argv[1];
			--argc;
			++argv;
		} 
		else if(!strcmp("-t", *argv) && argc > 1) 
		{
			trace_file_ = argv[1];
			--argc;
			++argv;
		} 
		else if(**argv != '-') 
			load_file_ = *argv;
		else if(!strcmp("--help", *argv)) 
		{
			 msg << "Usage: " << self << " [OPTIONS] [FILE]\n"
					"\n"
					"OPTIONS\n"
					"  -h              Halt CPU\n"
					"  -x              Exit on HALT\n"
					"  -b              Enter RX02 double density bootstrap program\n"
					"  -l file.bin     Load file.bin in absolute loader format\n"
					"  -c file		   Use configuration from specified ini file\n"
					"  -t file.trc     Record execution trace to file.trc\n"
					"  -z              Use delta compression for execution trace\n"				
					"  -q              Quiet mode\n"
					"\n"
					"The optional last argument FILE is equivalent to -f file";
			 throw (msg.str());
		} 
		else 
		{
			msg << "Unknown option: " << *argv;
			throw (msg.str());
		}
	}
}
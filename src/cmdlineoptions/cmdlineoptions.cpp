#include "cmdlineoptions.h"

#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;

CmdLineOptions::CmdLineOptions(int argc, char **argv)
{
	stringstream msg;

	// First argument is the executable's name
	const char* self = *argv;
    --argc;
	++argv;

	for (; argc; --argc, ++argv)
	{
		if (!strcmp("-h", *argv))
			halt = 1;
		else if(!strcmp("-b", *argv)) 
			bootstrap = 1;
		else if(!strcmp("-z", *argv)) 
			compress = 1;
		else if(!strcmp("-x", *argv))
			exit_on_halt = 1;
		else if(!strcmp("-l", *argv) && argc > 1) 
		{
			load_file = argv[1];
			--argc;
			++argv;
		} 
		else if(!strcmp("-f", *argv) && argc > 1) 
		{
			floppy_filename = argv[1];
			--argc;
			++argv;
		} 
		else if(!strcmp("-t", *argv) && argc > 1) 
		{
			trace_file = argv[1];
			--argc;
			++argv;
		} 
		else if(**argv != '-') 
			load_file = *argv;
		else if(!strcmp("--help", *argv)) 
		{
			 msg << "Usage: " << self << " [OPTIONS] [FILE]\n"
					"\n"
					"OPTIONS\n"
					"  -h              Halt CPU\n"
					"  -x              Exit on HALT\n"
					"  -b              Enter RX02 double density bootstrap program\n"
					"  -l file.bin     Load file.bin in absolute loader format\n"
					"  -f file.rx2     Load RX02 floppy image from file.rx2\n"
					"  -t file.trc     Record execution trace to file.trc\n"
					"  -z              Use delta compression for execution trace\n"
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
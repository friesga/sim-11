#include "cmdlineoptions.h"

#include <string>
#include <sstream>
#include <cstring>
#include <iostream>

using std::invalid_argument;

CmdLineOptions::CmdLineOptions(int argc, char const **argv) 
    :
    load_file {load_file_},
    config_file {config_file_},
    trace_file {trace_file_}
{
   	std::stringstream msg;

	// First argument is the executable's name
	const char* self = *argv;
    --argc;
	++argv;

	for (; argc; --argc, ++argv)
	{
		if(!strcmp("-l", *argv) && argc > 1) 
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
		else if(!strcmp("-h", *argv)) 
		{
			 msg << "Usage: " << self << " [Options]\n"
					"\n"
					"Options:\n"
					"  -l file.bin   Load file.bin in absolute loader format\n"
					"  -c file       Use configuration from specified ini file\n"
					"  -t file.trc   Record execution trace to file.trc\n"
					"  -h            Show this message";			
			 throw invalid_argument (msg.str());
		} 
		else 
		{
			msg << "Unknown option: " << *argv;
			throw invalid_argument (msg.str());
		}
	}
};
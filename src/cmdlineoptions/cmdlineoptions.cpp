#include "cmdlineoptions.h"

CmdLineOptions::CmdLineOptions() 
    :
    floppy_filename {floppy_filename_},
    load_file {load_file_},
    config_file {config_file_},
    halt {halt_} ,
    bootstrap {bootstrap_},
    trace_file {trace_file_},
    compress {compress_},
    exit_on_halt {exit_on_halt_},
    quiet {quiet_}
{};
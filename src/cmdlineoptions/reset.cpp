#include "cmdlineoptions.h"

void CmdLineOptions::reset ()
{
    halt_ = false;
    floppy_filename_ = nullptr;
    load_file_ = nullptr;
    bootstrap_ = false;
    trace_file_ = nullptr;
    compress_ = false;
    exit_on_halt_ = false;
    quiet_ = false;
}
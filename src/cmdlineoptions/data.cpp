#include "cmdlineoptions.h"

int CmdLineOptions::halt_ = 0;
const char *CmdLineOptions::floppy_filename_ = nullptr;
const char *CmdLineOptions::load_file_ = nullptr;
int CmdLineOptions::bootstrap_ = 0;
const char *CmdLineOptions::trace_file_ = nullptr;
int CmdLineOptions::compress_ = 0;
int CmdLineOptions::exit_on_halt_ = 0;
int CmdLineOptions::readOnly_ = 0;
int CmdLineOptions::quiet_ = 0;
int CmdLineOptions::createNewFile_ = 0;
int CmdLineOptions::openExistingFile_ = 0;
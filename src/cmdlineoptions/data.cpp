#include "cmdlineoptions.h"

bool CmdLineOptions::halt_ = 0;
const char *CmdLineOptions::floppy_filename_ = nullptr;
const char *CmdLineOptions::load_file_ = nullptr;
bool CmdLineOptions::bootstrap_ = 0;
const char *CmdLineOptions::trace_file_ = nullptr;
bool CmdLineOptions::compress_ = 0;
bool CmdLineOptions::exit_on_halt_ = 0;
bool CmdLineOptions::readOnly_ = 0;
bool CmdLineOptions::quiet_ = 0;
bool CmdLineOptions::createNewFile_ = 0;
bool CmdLineOptions::openExistingFile_ = 0;
#include "cmdlineoptions.h"

bool CmdLineOptions::halt_ = false;
const char *CmdLineOptions::floppy_filename_ = nullptr;
const char *CmdLineOptions::load_file_ = nullptr;
bool CmdLineOptions::bootstrap_ = false;
const char *CmdLineOptions::trace_file_ = nullptr;
bool CmdLineOptions::compress_ = false;
bool CmdLineOptions::exit_on_halt_ = false;
bool CmdLineOptions::readOnly_ = false;
bool CmdLineOptions::quiet_ = false;
bool CmdLineOptions::createNewFile_ = false;
bool CmdLineOptions::overwrite_ = false;
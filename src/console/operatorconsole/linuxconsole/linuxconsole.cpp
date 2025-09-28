#include "linuxconsole.h"

LinuxConsole::LinuxConsole ()
{
    senderThread_ = std::thread (&LinuxConsole::sender, this);
}

LinuxConsole::~LinuxConsole ()
{
    consoleRunning_ = false;
    senderThread_.join ();
}

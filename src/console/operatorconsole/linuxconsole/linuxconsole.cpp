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

// The onChar() function is only used by the WASM console,
// as the Linux console reads characters directly in the sender() function. 
void LinuxConsole::onChar (int ch)
{}

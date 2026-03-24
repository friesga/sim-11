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

// Print the given character on the operator's console
void LinuxConsole::print (char const c)
{
	// Just print 7-bit ASCII characters. The XXDP diagnostic VKABB0 e.g.
	// prints characters 0377 at the end of a string which stops WSL from
	// outputting further characters.
	printf ("%c", c & 0177);
	fflush (stdout);
}

// The onChar() function is only used by the WASM console,
// as the Linux console reads characters directly in the sender() function. 
void LinuxConsole::onChar (int ch)
{}

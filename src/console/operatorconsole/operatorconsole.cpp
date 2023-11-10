#include "operatorconsole.h"

#ifdef _WIN32
#include "windowsconsole/windowsconsole.h"
#else
#include "linuxconsole/linuxconsole.h"
#endif

#include <memory>

OperatorConsole::OperatorConsole ()
{
    // Create a thread running the event handler and rendering
    senderThread_ = std::thread (&OperatorConsole::sender, this);
}

OperatorConsole::~OperatorConsole ()
{
    consoleRunning_ = false;
    senderThread_.join ();
}

void OperatorConsole::setReceiver (std::function<void(char)> receiver)
{
    receiver_ = receiver;
}

bool OperatorConsole::isActive ()
{
    return consoleRunning_;
}

void OperatorConsole::send (const char c)
{
	receiver_ (c);
}

void OperatorConsole::sendString (const char* s)
{
	for(; *s; s++)
	{
		send(*s);
	}
}

// Print the given character on the operator's console
void OperatorConsole::print (char const c)
{
	// Just print 7-bit ASCII characters. The XXDP diagnostic VKABB0 e.g.
	// prints characters 0377 at the end of a string which stops WSL from
	// outputting further characters.
	printf ("%c", c & 0177);
	fflush (stdout);
}

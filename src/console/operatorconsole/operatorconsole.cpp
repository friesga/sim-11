#include "operatorconsole.h"

#ifdef _WIN32
#include "windowsconsole/windowsconsole.h"
#else
#include "linuxconsole/linuxconsole.h"
#endif

#include <memory>

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


#include "console.h"

#ifdef _WIN32
#include "windowsconsole/windowsconsole.h"
#else
#include "linuxconsole/linuxconsole.h"
#endif

#include <memory>

// Create a Windows or Linux variant of the console class
std::unique_ptr<Console> Console::create (DLV11J &dlv11)
{
#ifdef _WIN32
    return std::make_unique<WindowsConsole> (dlv11);
#else
    return std::make_unique<LinuxConsole> (dlv11);
#endif
}

Console::Console (DLV11J &dlv11)
    : dlv11_ {dlv11}
{
}

Console::~Console ()
{
    readerThread_.join ();
}

// Finish reading the console
void Console::finish ()
{
    consoleRunning_ = false;
}

bool Console::isRunning ()
{
    return consoleRunning_;
}


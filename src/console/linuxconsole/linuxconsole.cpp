#include "linuxconsole.h"

// The constructor starts a thread handling the console input and forwarding
// the read characters to the DLV11-J.
// ToDo: Merge WindowsConsole and LinuxConsole common functions
LinuxConsole::LinuxConsole (DLV11J &dlv11)
    : Console (dlv11)
{
    // Create a thread running the event handler and rendering
    readerThread_ = std::thread (&LinuxConsole::reader, this);
}


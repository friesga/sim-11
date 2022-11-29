#include "windowsconsole.h"

// The constructor starts a thread handling the console input and forwarding
// the read characters to the DLV11-J.
WindowsConsole::WindowsConsole (DLV11J &dlv11)
    : Console (dlv11)
{
     dlv11_ = dlv11;

    // Create a thread running the event handler and rendering
    readerThread_ = std::thread (&WindowsConsole::reader, this);
}


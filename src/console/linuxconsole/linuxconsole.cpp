#include "linuxconsole.h"

// The constructor starts a thread handling the console input and forwarding
// the read characters to the DLV11-J.
LinuxConsole::LinuxConsole (DLV11J &dlv11)
    : Console (dlv11)
{}


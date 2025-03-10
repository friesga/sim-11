#include "windowsconsole.h"

#include <string>
#include <iostream>
#include <stdexcept>

using std::runtime_error;

void WindowsConsole::sender ()
{
    HANDLE stdInputHandle;
    HANDLE stdOutputHandle;

    consoleRunning_ = true;

    // Get handle to stdin
    if ((stdInputHandle = 
            GetStdHandle (STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
        throw runtime_error ("Cannot get console input handle");

    // Set console in raw mode
    if (!SetConsoleMode (stdInputHandle, 0))
        throw runtime_error ("Setting console mode failure");

    // Get handle to stdout
    if ((stdOutputHandle =
        GetStdHandle (STD_OUTPUT_HANDLE)) == INVALID_HANDLE_VALUE)
        throw runtime_error ("Cannot get console output handle");

    // Set console to VT100 mode.
    if (!SetConsoleMode (stdOutputHandle,
            ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        throw runtime_error ("Setting console mode failure");

    while (consoleRunning_)
    {
        char c;

        // Read one character from the console and pass it to the DLV11J's
        // receiver
		if (readCharacter (stdInputHandle, &c, 1))
            send (c);
    }
}
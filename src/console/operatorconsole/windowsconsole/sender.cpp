#include "windowsconsole.h"

#include <string>
#include <iostream>

void WindowsConsole::sender ()
{
    HANDLE stdInputHandle;

    consoleRunning_ = true;

    // Get handle to stdin
    if ((stdInputHandle = 
            GetStdHandle (STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
        throw std::string ("Cannot get console input handle");

    // Set console in raw mode
    if (!SetConsoleMode (stdInputHandle, 0))
        throw std::string("Setting console mode failure");

    while (consoleRunning_)
    {
        char c;

        // Read one character from the console and pass it to the DLV11J's
        // receiver
		if (readCharacter (stdInputHandle, &c, 1))
            send (c);
    }
}
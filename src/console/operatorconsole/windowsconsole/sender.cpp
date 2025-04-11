#include "windowsconsole.h"

#include <string>
#include <iostream>
#include <stdexcept>

using std::runtime_error;
using std::cerr;

void WindowsConsole::sender ()
try
{
    consoleRunning_ = true;

    while (consoleRunning_)
    {
        char c;

        // Read one character from the console and pass it to the DLV11J's
        // receiver
		if (readCharacter (stdInputHandle_, &c, 1))
            send (c);
    }
}
catch (const std::exception& ex)
{
    cerr << "WindowsConsole::sender exception: " << ex.what () << '\n';
}
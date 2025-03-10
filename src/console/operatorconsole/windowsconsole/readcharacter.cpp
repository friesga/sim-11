#include "windowsconsole.h"

#include <iostream>
#include <string>
#include <stdexcept>

using std::runtime_error;
using std::cerr;

// Read one character from console input
bool WindowsConsole::readCharacter (HANDLE hStdin, char* buffer, int count)
try
{
    INPUT_RECORD irBuffer[128];
    DWORD cNumRead;

    while (consoleRunning_)
    {
        // Wait 50ms for a character to become available
        switch (WaitForSingleObject (hStdin, 50))
        {
            case WAIT_OBJECT_0:
                // Read one character from the input buffer
                if (!ReadConsoleInput (hStdin, irBuffer, 1, &cNumRead))
                    return false;

                if (getAsciiChar (&irBuffer[0], buffer))
                    return true;
                break;

            case WAIT_TIMEOUT:
                // Read time-out check if reading the console has to finish
                break;

            case WAIT_FAILED:
                throw runtime_error ("WaitForSingleObject failed");
                break;

            case WAIT_ABANDONED:
                throw runtime_error ("WaitForSingleObject abandoned");

            default:
                throw runtime_error ("Unexpected return from WaitForSingleObject");
                break;
        }
    }
    return false;
}
catch (const std::exception& ex)
{
    cerr << "WindowsConsole::readCharacter exception: " << ex.what () << '\n';
    return false;
}
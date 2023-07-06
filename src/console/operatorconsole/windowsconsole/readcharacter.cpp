#include "windowsconsole.h"

#include <string>

// Read one character from console input

bool WindowsConsole::readCharacter (HANDLE hStdin, char* buffer, int count)
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
                throw std::string ("WaitForSingleObject failed");
                break;

            case WAIT_ABANDONED:
                throw std::string ("WaitForSingleObject abandoned");

            default:
                throw std::string ("Unexpected return from WaitForSingleObject");
                break;
        }
    }
    return false;
}
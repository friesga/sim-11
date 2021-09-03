#include <windows.h>
#include <stdio.h>
#include <termio.h>

int isValidKeyEvent (INPUT_RECORD*);

HANDLE open_stdin ()
{
    HANDLE stdInputHandle;

    // Get the standard input handle.
    if ((stdInputHandle = GetStdHandle (STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
        return NULL;

    return stdInputHandle;
}

int select_stdin (HANDLE hStdin)
{
    INPUT_RECORD irInBuf[128];
    DWORD cNumRead;
    DWORD i;

    // Check if input is available
    if (!PeekConsoleInput (hStdin, irInBuf, 128, &cNumRead))
        return 1;

    if (cNumRead > 0)
    {
        // Check if a key down KEY_EVENT for a "printable" key is present in the input buffer
        for (i = 0; i < cNumRead; i++)
        {
            if (isValidKeyEvent (&irInBuf[i]))
                return 1;
        }
    }

    return 0;
}

/* Read one character from console input */

int read_stdin (HANDLE hStdin, char* buffer, int count)
{
    INPUT_RECORD irBuffer[128];
    DWORD cNumRead;

    while (1)
    {
        // Read one character from the input buffer
        if (!ReadConsoleInput (hStdin, irBuffer, 1, &cNumRead))
            return 0;

        if (isValidKeyEvent (&irBuffer[0]))
        {
            *buffer = irBuffer[0].Event.KeyEvent.uChar.AsciiChar;
            return 1;
        }
    }
}

int isValidKeyEvent (INPUT_RECORD* input_record)
{
    if ((input_record->EventType == KEY_EVENT) &&
        (input_record->Event.KeyEvent.bKeyDown) &&
        (input_record->Event.KeyEvent.uChar.AsciiChar > 0))
        return 1;

    return 0;
}

int getattr_stdin (HANDLE hStdin)
{
    return 0;
}

int setattr_stdin (HANDLE hStdin)
{
    // Enable the window events
    // ENABLE_PROCESSED_INPUT captures CTRL-C by the system
    if (!SetConsoleMode (hStdin, ENABLE_WINDOW_INPUT | ENABLE_PROCESSED_INPUT))
        return -1;
    return 0;
}

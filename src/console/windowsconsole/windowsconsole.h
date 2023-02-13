#ifndef _WINDOWS_CONSOLE_H_
#define _WINDOWS_CONSOLE_H_

#include "../console.h"

#include <windows.h>
#include <stdio.h>

// Implementation of the Console reader function for Windows.
class WindowsConsole : public Console
{
    bool readCharacter (HANDLE hStdin, char* buffer, int count);
    bool isValidKeyEvent (INPUT_RECORD* input_record);

public:
    WindowsConsole (DLV11J *dlv11);
    void reader ();
};


#endif // _WINDOWS_CONSOLE_H_
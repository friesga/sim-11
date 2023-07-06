#ifndef _WINDOWS_CONSOLE_H_
#define _WINDOWS_CONSOLE_H_

#include "../operatorconsole.h"

#include <windows.h>
#include <stdio.h>

// Implementation of the Console reader function for Windows.
class WindowsConsole : public OperatorConsole
{
    bool shiftKeyPressed {false};

    bool readCharacter (HANDLE hStdin, char* buffer, int count);
    bool getAsciiChar (INPUT_RECORD* input_record, char *c);

public:
    void sender () override;
};

#endif // _WINDOWS_CONSOLE_H_
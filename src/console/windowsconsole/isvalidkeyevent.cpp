#include "windowsconsole.h"

bool WindowsConsole::isValidKeyEvent (INPUT_RECORD* input_record)
{
    if ((input_record->EventType == KEY_EVENT) &&
        (input_record->Event.KeyEvent.bKeyDown) &&
        (input_record->Event.KeyEvent.uChar.AsciiChar > 0))
        return true;

    return false;
}
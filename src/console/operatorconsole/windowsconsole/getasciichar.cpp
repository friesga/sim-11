#include "windowsconsole.h"

// Get the ASCII character from the given input record if one is available.
// The Window Console API makes it ridiculously difficult to process
// diacritic characters as normal ASCII characters. These diacritics are
// dead-keys and are combined with the next key entered to a Unicode
// character. For ODT however we need the caret (up-arrow) character as a
// regular character. To that end we check the combination of the shift key
// and the '6' key pressed and convert that to the caret character. To get
// rid of the combined character we skip key events that have no scan code
// associated. There should be a better solution to this issue but that's
// not documented.
//
bool WindowsConsole::getAsciiChar (INPUT_RECORD* input_record, char *c)
{
    if (input_record->EventType == KEY_EVENT &&
        input_record->Event.KeyEvent.bKeyDown &&
        input_record->Event.KeyEvent.wVirtualScanCode > 0)
    {
        if (input_record->Event.KeyEvent.uChar.AsciiChar > 0)
        {
            *c = input_record->Event.KeyEvent.uChar.AsciiChar;
            return true;
        }

        if (input_record->Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED &&
            input_record->Event.KeyEvent.wVirtualKeyCode == 0x36)
        {
            *c = '^';
            return true;
        }
    }
    return false;
}
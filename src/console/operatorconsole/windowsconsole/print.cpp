#include "windowsconsole.h"

#include <iostream>

using std::cerr;

// Print the given character on the windows console
void WindowsConsole::print (char const c)
{
    // If the WriteConsole() we probably cannot print to stderr either
    // but at least we can try to indicate the error.
    if (!WriteConsole (stdOutputHandle_, &c, 1, nullptr, nullptr))
        cerr << "WriteConsole failed\n";
}
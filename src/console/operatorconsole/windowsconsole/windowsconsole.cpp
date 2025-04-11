#include "windowsconsole.h"

#include <iostream>
#include <stdexcept>

using std::runtime_error;
using std::cerr;

WindowsConsole::WindowsConsole ()
try
{
    // Get handle to stdin
    if ((stdInputHandle_ =
        GetStdHandle (STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE)
        throw runtime_error ("Cannot get console input handle");

    // Set console in raw mode
    if (!SetConsoleMode (stdInputHandle_, 0))
        throw runtime_error ("Setting console mode failure");

    // Get handle to stdout
    if ((stdOutputHandle_ =
        GetStdHandle (STD_OUTPUT_HANDLE)) == INVALID_HANDLE_VALUE)
        throw runtime_error ("Cannot get console output handle");

    // Set console to VT100 mode.
    if (!SetConsoleMode (stdOutputHandle_,
        ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        throw runtime_error ("Setting console mode failure");
}
catch (const std::exception& ex)
{
    cerr << "WindowsConsole::WindowsConsole exception: " << ex.what () << '\n';
}
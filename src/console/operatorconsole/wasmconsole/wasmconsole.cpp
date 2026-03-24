#include "wasmconsole.h"

#include <emscripten.h>

WASMConsole::WASMConsole ()
{
    WASMConsoleInputRouter::instance ().attach (this);
}

WASMConsole::~WASMConsole ()
{
    WASMConsoleInputRouter::instance ().detach (this);
}

// We don't need to start a separate thread for the sender function in the
// WASM console as the main loop is handled by emscripten and the
// onChar() function is called from there.
void WASMConsole::sender ()
{
}

void WASMConsole::print (char const c)
{
    // The variable "window" can only be referenced from the main thread and
    // as this function is callled from a separate thread, we have to make sure
    // the code is executed on the main thread.
    MAIN_THREAD_EM_ASM (
        {
            if (window.term)
            {
                window.term.write (String.fromCharCode ($0));
            }
        }, c & 0x7f);
}

void WASMConsole::onChar (int ch)
{
    send (ch & 0177);
}
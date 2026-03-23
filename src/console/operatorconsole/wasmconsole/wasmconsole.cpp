#include "wasmconsole.h"

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
{}

void WASMConsole::onChar (int ch)
{
    send (ch & 0177);
}
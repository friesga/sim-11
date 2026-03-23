#include "wasmconsoleinputrouter.h"

extern "C" void on_xterm_char (int ch)
{
    WASMConsoleInputRouter::instance ().onChar (ch);
}

WASMConsoleInputRouter& WASMConsoleInputRouter::instance ()
{
    static WASMConsoleInputRouter r;
    return r;
}

void WASMConsoleInputRouter::attach (Console* console)
{
    console_ = console;
}

void WASMConsoleInputRouter::detach (Console* console)
{
    console_ = nullptr;
}

void WASMConsoleInputRouter::onChar (int ch)
{
    if (console_ != nullptr)
        console_->onChar (static_cast<char> (ch));
}
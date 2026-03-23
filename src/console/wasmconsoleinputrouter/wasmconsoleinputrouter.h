#ifndef _WASMCONSOLEINPUTROUTER_H_
#define _WASMCONSOLEINPUTROUTER_H_

#include "console/console.h"

// This class routes character input from the browser to the WASM console.
class WASMConsoleInputRouter
{
public:
    static WASMConsoleInputRouter& instance ();
    void attach (Console* console);
    void detach (Console* console);
    void onChar (int ch);

private:
    Console* console_ = nullptr;
};


#endif // _WASMCONSOLEINPUTROUTER_H_
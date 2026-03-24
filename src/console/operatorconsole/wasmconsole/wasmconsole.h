#ifndef _WASM_CONSOLE_H_
#define _WASM_CONSOLE_H_

#include "console/operatorconsole/operatorconsole.h"
#include "console/wasmconsoleinputrouter/wasmconsoleinputrouter.h"

// Implementation of the Console reader function for WASM.
class WASMConsole : public OperatorConsole
{
public:
    WASMConsole ();
    ~WASMConsole ();
    void sender () override;
    void print (char const c) override;
    void onChar (int ch) override;
};

#endif // _WASM_CONSOLE_H_
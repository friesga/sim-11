#ifndef _WASM_CONSOLE_H_
#define _WASM_CONSOLE_H_

#include "../operatorconsole.h"

// Implementation of the Console reader function for WASM.
class WASMConsole : public OperatorConsole
{
public:
    void sender () override;

    void onChar (int ch);
};


#endif // _WASM_CONSOLE_H_
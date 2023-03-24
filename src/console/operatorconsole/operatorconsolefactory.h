#ifndef _OPERATORCONSOLEFACTORY_H_
#define _OPERATORCONSOLEFACTORY_H_

#include "../console.h"
#ifdef _WIN32
#include "windowsconsole/windowsconsole.h"
#else
#include "linuxconsole/linuxconsole.h"
#endif

#include <memory>

using std::make_unique;
using std::unique_ptr;

// This class is a factory creating a WindowConsole or a LinuxConsole,
// depending on the platform used.
class OperatorConsoleFactory
{
public:
    static unique_ptr<Console> create ()
    {
#ifdef _WIN32
        return make_unique<WindowsConsole> ();
#else
        return make_unique<LinuxConsole> ();
#endif
    }
};

#endif // _OPERATORCONSOLEFACTORY_H_
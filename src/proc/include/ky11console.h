#ifndef _KY11CONSOLE_H_
#define _KY11CONSOLE_H_

#include "busaddress/busaddress.h"
#include "conddata/conddata.h"
#include "types.h"

#include <variant>
#include <functional>

using std::variant;
using std::function;

// This class defines the interface for KY11 consoles
class KY11Console
{
public:
    struct StartPressed
    {
        u16 startAddress;
    };

    struct ContinuePressed
    {};

    struct HaltPressed
    {};

    using ConsoleEvent = variant<StartPressed, ContinuePressed, HaltPressed>;

    enum class HaltEnablePosition
    {
        Halt,
        Enable
    };

    virtual HaltEnablePosition haltEnablePosition () const = 0;
    virtual void display (BusAddress address, CondData<u16> data) = 0;
};

#endif // _KY11CONSOLE_H_
#ifndef _KY11CONSOLE_H_
#define _KY11CONSOLE_H_

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
    using Subscriber = function<void (ConsoleEvent)>;

    enum class HaltEnablePosition
    {
        Halt,
        Enable
    };

    virtual void subscribe (Subscriber subscriber) = 0;
    virtual HaltEnablePosition haltEnablePosition () const = 0;
};

#endif // _KY11CONSOLE_H_
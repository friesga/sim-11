#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <functional>

using std::function;

// This class defines the Console interface to be passed to and used by DLV11J.
class Console
{
public:
    virtual void setReceiver (function<void(int, char)>) = 0;
    virtual void send (char) = 0;
    virtual void print (char const c) = 0;
    virtual bool isActive() = 0;
    virtual ~Console () {};
};

#endif // _CONSOLE_H_
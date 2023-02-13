#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "dlv11j/dlv11j.h"

#include <memory>
#include <thread>

using std::unique_ptr;

// // A class handling the simulator's console. As this requires unbuffered
// input the class is highly system dependent. Therefore a Windows and a
// Linux variant of this class have to be defined. All system dependent code
// is in WindowsConsole and LinuxConsole; the common code is in this Console
// class. Both console implementations provide the same interface, as defined
// in the Console class. 
// The create() functions creates a WindowConsole or a LinuxConsole,
// depending on the platform used.
class Console
{
protected:
    std::thread readerThread_;
    bool consoleRunning_ {false};
    DLV11J *dlv11_;

public:
    Console (DLV11J *dlv11);
    ~Console ();
    virtual void reader() = 0;
    bool isRunning();
    void send (const char c);
    void sendString (const char* s);

    static unique_ptr<Console> create (DLV11J *dlv11);
};

#endif // !_CONSOLE_H_

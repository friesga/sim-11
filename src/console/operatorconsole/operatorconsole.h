#ifndef _OPERATORCONSOLE_H_
#define _OPERATORCONSOLE_H_

#include "../console.h"

#include <memory>
#include <thread>
#include <functional>

// // A class handling the operator's console. As this requires unbuffered
// input the class is highly system dependent. Therefore a Windows and a
// Linux variant of this class have to be defined. All system dependent code
// is in WindowsConsole and LinuxConsole; the common code is in this Console
// class. Both console implementations provide the same interface, as defined
// in the OperatorConsole class. 
class OperatorConsole : public Console
{
public:
    OperatorConsole ();
    ~OperatorConsole ();
    virtual void sender () = 0;

    // Console functions to implement
    void setReceiver (std::function<void(char)> receiver) override;
    void send (const char c) override;
    void print (char const c) override;
    bool isActive() override;
    
    void sendString (const char* s);
;

protected:
    std::thread senderThread_;
    bool consoleRunning_ {false};
    std::function<void(char)> receiver_;
};

#endif // !_OPERATORCONSOLE_H_

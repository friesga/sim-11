#ifndef _OPERATORCONSOLE_H_
#define _OPERATORCONSOLE_H_

#include "../console.h"

#include <memory>
#include <thread>
#include <functional>

// // A class handling the operator's console. As this requires unbuffered
// input the class is highly system dependent. Therefore a Windows, a Linux
// and a WASM variant of this class are implemented. All system dependent code
// is in WindowsConsole, LinuxConsole and WASMConsole; the common code is in
// this OperatorConsole class. All console implementations provide the same
// interface, as defined in the OperatorConsole class. 
class OperatorConsole : public Console
{
public:
    virtual void sender () = 0;

    // Console functions to implement
    void setReceiver (std::function<void(char)> receiver) override;
    void send (const char c) override;
    void print (char const c) override;
    bool isActive() override;
    
    void sendString (const char* s);

protected:
    std::thread senderThread_;
    bool consoleRunning_ {false};
    std::function<void(char)> receiver_;
};

#endif // !_OPERATORCONSOLE_H_

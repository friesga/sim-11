#ifndef _LINUX_CONSOLE_H_
#define _LINUX_CONSOLE_H

#include "../console.h"

// Implementation of the Console reader function for Linux.
class LinuxConsole : public Console
{
public:
    LinuxConsole (std::shared_ptr<DLV11J> dlv11);
    void reader ();
};

#endif // !_LINUX_CONSOLE_H_

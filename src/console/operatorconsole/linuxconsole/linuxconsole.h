#ifndef _LINUX_CONSOLE_H_
#define _LINUX_CONSOLE_H_

#include "../operatorconsole.h"

// Implementation of the Console reader function for Linux.
class LinuxConsole : public OperatorConsole
{
public:
    LinuxConsole ();
    ~LinuxConsole ();
    void sender () override;
    void onChar (int ch) override;
};

#endif // !_LINUX_CONSOLE_H_

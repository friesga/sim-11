#ifndef _LINUX_CONSOLE_H_
#define _LINUX_CONSOLE_H

#include "../operatorconsole.h"

// Implementation of the Console reader function for Linux.
class LinuxConsole : public OperatorConsole
{
public:
    void sender () override;
};

#endif // !_LINUX_CONSOLE_H_

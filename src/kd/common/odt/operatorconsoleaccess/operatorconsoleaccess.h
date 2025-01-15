#ifndef _OPERATORCONSOLEACCESS_H_
#define _OPERATORCONSOLEACCESS_H_

#include "kd/include/consoleaccess.h"
#include "bus/bus.h"
#include "conddata/conddata.h"

// This class handles character reads and writes to the console DLV11-J
class OperatorConsoleAccess : public ConsoleAccess
{
public:
    OperatorConsoleAccess (Bus* bus);
    bool available ();
	CondData<u8> read ();
    void write (u8 c);

private:
    Bus* bus_;

    CondData<u16> readDLV11J (u16 address);
    bool transmitReady ();
};


#endif // !_OPERATORCONSOLEACCESS_H_

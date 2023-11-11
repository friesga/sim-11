#ifndef _OPERATORCONSOLEACCESS_H_
#define _OPERATORCONSOLEACCESS_H_

#include "../consoleaccess.h"
#include "qbus/qbus.h"
#include "conddata/conddata.h"

// This class handles character reads and writes to the console DLV11-J
class OperatorConsoleAccess : public ConsoleAccess
{
public:
    OperatorConsoleAccess (Qbus* bus);
    bool available ();
	CondData<u8> read ();
    void write (u8 c);

private:
    Qbus* bus_;

    CondData<u16> readDLV11J (u16 address);
    bool transmitReady ();
};


#endif // !_OPERATORCONSOLEACCESS_H_

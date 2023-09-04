#ifndef _CONSOLEACCESS_H_
#define _CONSOLEACCESS_H_

#include "qbus/qbus.h"
#include "conddata/conddata.h"

// This class defines the interface to a Console.
class ConsoleAccess
{
public:
    virtual bool available () = 0;
	virtual CondData<u8> read () = 0;
    virtual void write (u8 c) = 0;
};

#endif // _CONSOLEACCESS_H_
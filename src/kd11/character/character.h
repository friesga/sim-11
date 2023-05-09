#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "qbus/qbus.h"
#include "conddata/conddata.h"

// This class handles character reads and writes to the console DLV11-J
class Character
{
public:
    Character (Qbus* bus);
    bool available ();
	CondData<u8> read ();
    void write (u8 c);

private:
    Qbus* bus_;

    CondData<u16> readDLV11J (u16 address);
};


#endif // !_CHARACTER_H_

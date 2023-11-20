#ifndef _KTF11A_H_
#define _KTF11A_H_

#include "qbus/qbus.h"

// The class KTF11_A implements the memory management option for the KDF11-A.
// It implements a subset of the standard PDP-11 Memory Management. There is
// no support for split I/D space and no supervisor mode.
//
class KTF11_A
{
public:
	KTF11_A (Qbus* bus);
    CondData<u16> read (u16 address);
	bool writeWord (u16 address, u16 value);
	bool writeByte (u16 address, u8 value);

private:
	Qbus* bus_;
};

#endif _KTF11A_H_
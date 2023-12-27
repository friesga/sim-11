#ifndef _MMU_H_
#define _MMU_H_

#include "types.h"
#include "conddata/conddata.h"
#include "psw.h"

// This class defines the functions a Memory Management Unit has to provide.
class MMU
{
public:
	virtual void reset () = 0;
    virtual CondData<u16> fetchWord (u16 address, 
		PSW::Mode memMgmtMode = PSW::Mode::Default) = 0;
	virtual CondData<u8> fetchByte (u16 address, 
		PSW::Mode memMgmtMode = PSW::Mode::Default) = 0;
	virtual bool putWord (u16 address, u16 value, 
		PSW::Mode memMgmtMode = PSW::Mode::Default) = 0;
	virtual bool putByte (u16 address, u8 value,
		PSW::Mode memMgmtMode = PSW::Mode::Default) = 0;
	virtual bool pushWord (u16 value) = 0;
	virtual bool popWord (u16 *destination) = 0;

	virtual void setVirtualPC (u16 value) = 0;
	virtual CondData<u16> readWithoutTrap (u16 address) = 0;
};


#endif // _MMU_H_
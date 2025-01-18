#ifndef _BUSDATATRANSFER_H_
#define _BUSDATATRANSFER_H_

#include "busaddress/busaddress.h"
#include "conddata/conddata.h"

// This interface declares the bus functions for data transfer
class BusDataTransfer
{
public:
	
	virtual CondData<u16> read (BusAddress address) = 0;
	virtual bool writeWord (BusAddress address, u16 value) = 0;
	virtual bool writeByte (BusAddress address, u8 val) = 0;
};

#endif // _BUSDATATRANSFER_H_
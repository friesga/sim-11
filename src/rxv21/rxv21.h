#ifndef _RXV21_H_
#define _RXV21_H_

#include "../qbus/qbus.h"

class RXV21 : public QBUSModule
{
public:
	RXV21 ();
	~RXV21 ();
	u16 read (u16 address);
	void write (u16 address, u16 value);
	u8 responsible (u16 address);
	void reset ();
	void setData (u8* data);
	void step();

private:
	void clearErrors ();
	void done ();
	void fillBuffer ();
	void emptyBuffer ();
	void writeSector ();
	void readSector ();
	void readStatus ();
	void readErrorCode ();
	void initiateCommand ();
	void process ();
	InterruptRequest interruptRequest(unsigned char vector);

	u16	base;
	u16	vector;
	InterruptRequest const emptyIntrptReq{TrapPriority::None, 0, 0};

	// All software control of the RX211/RXV21 is performed by means
	// of two device registers: the command and status register (RX2CS)
	// and a multipurpose data buffer register (RX2DB) which have been
	// assigned bus addresses and can be read or loaded.
	u16	rx2cs;	// Control and status register
	u16	rx2db;	// Data Buffer register

	// Contents of the RXV21's "virtual" registers.
	// Note that the term "register" is a misnomer, the contents of
	// these "registers" are read and written via the Data Buffer (RX2DB)
	// register.
	u16	rx2ta;	// RX Track Address
	u16	rx2sa;	// RX Sector Address
	u16	rx2wc;	// RX Word Count Register
	u16	rx2ba;	// RX Bus Address Register
	u16	rx2es;	// RX Error and Status

	u16	state;
	u16	error;

	u16	buffer[128];
	u8* data;
};


#endif // _RXV21_H_
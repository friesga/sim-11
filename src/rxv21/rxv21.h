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
	void executeCommand ();
	void process ();

	u16	base;
	u16	vector;

	u16	rx2cs;
	u16	rx2db;

	u16	rx2ta;	/* RX Track Address */
	u16	rx2sa;	/* RX Sector Address */
	u16	rx2wc;	/* RX Word Count Register */
	u16	rx2ba;	/* RX Bus Address Register */
	u16	rx2es;	/* RX Error and Status */

	u16	state;
	u16	error;

	u16	buffer[128];
	u8* data;
};


#endif _RXV21_H_
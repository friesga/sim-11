#ifndef _BDV11_H_
#define _BDV11_H_

#include "../qbus/qbus.h"

/* LTC rate */
#define	LTC_RATE		50
#define	LTC_TIME		(1.0F / LTC_RATE)

class BDV11 : public QBUSModule
{
public:
	BDV11 ();
	~BDV11 ();
	u16 read (u16 address);
	void write (u16 address, u16 value);
	u8 responsible (u16 address);
	void reset ();
	void step (float dt);

private:
	u16 getWordLow (u16 word);
	u16 getWordHigh (u16 word);
	void memoryDump (u16 pcr, int hi);

	u16	pcr;
	u16	scratch;
	u16	option;
	u16	display;
	u16	ltc;
	float	time;
};


extern const u16 bdv11_e53[2048];


#endif // !_BDV11_H_
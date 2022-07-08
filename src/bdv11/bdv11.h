#ifndef _BDV11_H_
#define _BDV11_H_

#include "qbus/qbus.h"

/* LTC rate */
// The default RT-11 line clock frequency is 60 Hz
#define	LTC_RATE		60

class BDV11 : public QBUSModule
{
public:
	BDV11 ();
	~BDV11 ();
	StatusCode read (u16 address, u16 *destAddress);
	void writeWord (u16 address, u16 value);
	u8 responsible (u16 address);
	void reset ();

private:
	u16 getWordLow (u16 word);
	u16 getWordHigh (u16 word);
	void memoryDump (u16 pcr, int hi);
	void tick();

	u16	pcr;
	u16	scratch;
	u16	option;
	u16	display;
	u16	ltc;
	float	time;
	std::thread ltcThread_;
	bool running_;
	InterruptRequest eventIntrptReq{RequestType::IntrptReq, TrapPriority::Event, 0, 0100};
};

extern const u16 bdv11_e53[2048];

#endif // !_BDV11_H_
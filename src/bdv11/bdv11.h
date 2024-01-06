#ifndef _BDV11_H_
#define _BDV11_H_

#include "qbus/qbus.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "configdata/bdv11config/bdv11config.h"

#include <memory>

using std::shared_ptr;

/* LTC rate */
// The default RT-11 line clock frequency is 60 Hz
#define	LTC_RATE		60

class BDV11 : public PDP11Peripheral
{
public:
	BDV11 (Qbus *bus);
	BDV11 (Qbus *bus, shared_ptr<BDV11Config> bdv11Config);
	~BDV11 ();
	StatusCode read (BusAddress address, u16 *destAddress) override;
	StatusCode writeWord (BusAddress address, u16 value) override;
	bool responsible (BusAddress address) override;
	void reset ();

	// Declare the signal receivers
	void BINITReceiver (bool signalValue);
	void BPOKReceiver (bool signalValue);

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
	u16 switchRegister_;
	float	time;
	std::thread ltcThread_;
	bool running_;
};

extern const u16 bdv11_e53[2048];

#endif // !_BDV11_H_
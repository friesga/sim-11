#ifndef _BDV11_H_
#define _BDV11_H_

#include "qbus/qbus.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "configdata/bdv11config/bdv11config.h"

#include <memory>
#include <array>
#include <map>
#include <string>

using std::shared_ptr;
using std::array;
using std::map;
using std::string;

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
	static const u16 PCR {0177520};		// Page Control Register
	static const u16 RWR {0177522};		// Read/Write Register
	static const u16 SDR {0177524};		// Switch and Display Register
	static const u16 BER {0177546};		// BEVNT Register

	using ROMimage = const array<u16, 2048>;

	ROMimage* romToUse (shared_ptr<BDV11Config> bdv11Config);
	u16 switchRegisterValue (shared_ptr<BDV11Config> bdv11Config);
	u16 getWordLow (u16 word);
	u16 getWordHigh (u16 word);
	void memoryDump (u16 pcr, int hi);
	void tick();

	static ROMimage ROM_045E2_046E2;
	static ROMimage ROM_339E2_340E2;

	map<BDV11Config::BootROM, ROMimage&> availableROMs =
	{
		{BDV11Config::BootROM::BDV11,    ROM_045E2_046E2},
		{BDV11Config::BootROM::KDF11_BA, ROM_339E2_340E2}
	};

	u16	pcr;
	u16	scratch;
	u16	option;
	u16	display;
	u16	ltc;
	u16 switchRegister_;
	float	time;
	std::thread ltcThread_;
	bool running_;
	ROMimage* romUsed_ {&ROM_045E2_046E2};
};

#endif // !_BDV11_H_
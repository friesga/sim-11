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

// Definition of the Switch Register lay-out.
// The switch register is used for maintenance and system configuration
// (selects diagnostic and/or bootstrap programs for execution). Bits 0-11
// of the register corresponding with switches E15-1 through E15-8 (hereafter
// named A1 through A8 and B1 through B8 respectively).
//
// Switches A1 through B4 are defined as follows:
// A1 0N	Execute CPU tests on power-up or restart.
// A2 ON	Execute memory test on power-up or restart.
// A3 ON	DECNET BOOT - A4, A5, A6, A7 are used as arguments.
// A4 ON	Console test and dialog (A3 OFF).
// A4 OFF	Turnkey BOOT dispatched by switch setting (A3 OFF).
//			Switches A5, A6, A7, A8, B1 are used as arguments.
// 
//			Device			A5	A6	A7	A8	B1
//			Loop on Error	OFF OFF OFF OFF ON
//			RK05			OFF	OFF OFF ON	OFF
//			RL01			OFF OFF ON	OFF	OFF
//			TU58			OFF OFF ON  ON  OFF
//			RX01			OFF ON	OFF OFF OFF
//			RX02			OFF ON	ON	OFF OFF
//			BDV11 ROM		ON	OFF	OFF	OFF OFF
//
// Notes:
// 1) The B-switches are only present on the BDV11.
// 2) The TU58 as boot device is not supported in the BDV11 ROMs.
// 
// Sources: LSI-11 Systems Service Manual, 3rd Edition, module M8012
// and KDF11-BA CPU Module Users's Guide.
//
consteval u16 A (size_t x)
{
    return 1 << (x - 1);
}

consteval u16 B (size_t x)
{
    return 1 << (x + 7);
}
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
	static const u16 LKS {0177546};		// Line Clock Status Register

	static const u16 LKS_IE		     {bitValue (6)};

	static const u16 BDV11_CPU_TEST	 {A(1)};
	static const u16 BDV11_MEM_TEST	 {A(2)};	
	static const u16 BDV11_DECNET	 {A(3)};
	static const u16 BDV11_DIALOG	 {A(4)};
	static const u16 BDV11_LOOP		 {B(1)};
	static const u16 BDV11_RK05		 {A(8)};
	static const u16 BDV11_RL01		 {A(7)};
	static const u16 BDV11_TU58		 {A(7) | A(8)};
	static const u16 BDV11_RX01		 {A(6)};
	static const u16 BDV11_RX02		 {A(6) | A(7)};
	static const u16 BDV11_ROM		 {A(5)};

	static const u16 BDV11_EXT_DIAG	 {B(2)};
	static const u16 BDV11_2780		 {B(3)};
	static const u16 BDV11_PROG_ROM	 {B(4)};

// Definition of the default contents of the switch register in case no
// configuration options are specified
	static const u16 BDV11_SWITCH	 {BDV11_CPU_TEST | BDV11_MEM_TEST |\
									  BDV11_DIALOG   | BDV11_RX02};

	using ROMimage = const array<u16, 2048>;

	ROMimage* romToUse (shared_ptr<BDV11Config> bdv11Config);
	u16 switchRegisterProgramSelection (shared_ptr<BDV11Config> bdv11Config);
	u16 switchRegisterBootDevice (shared_ptr<BDV11Config> bdv11Config);
	u16 getWordLow (u16 word);
	u16 getWordHigh (u16 word);
	void memoryDump (u16 pcr, int hi);
	void tick();
	void writeLKS (u16 value);

	static ROMimage ROM_045E2_046E2;
	static ROMimage ROM_339E2_340E2;

	// Mapping of the boot ROM specified in the configuration to the
	// array with the ROM contents.
	map<BDV11Config::BootROM, ROMimage&> availableROMs =
	{
		{BDV11Config::BootROM::BDV11,    ROM_045E2_046E2},
		{BDV11Config::BootROM::KDF11_BA, ROM_339E2_340E2}
	};

	// Mapping of the boot device specified in the configuration to the
	// corresponding value in the switch register.
	map <BDV11Config::BootDevice, u16> bootDevices =
	{
		{BDV11Config::BootDevice::RK05,     BDV11_RK05},
		{BDV11Config::BootDevice::RL01,     BDV11_RL01},
		{BDV11Config::BootDevice::TU58,     BDV11_TU58},
		{BDV11Config::BootDevice::RX01,     BDV11_RX01},
		{BDV11Config::BootDevice::RX02,     BDV11_RX02},
		{BDV11Config::BootDevice::BDV11ROM, BDV11_ROM}
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
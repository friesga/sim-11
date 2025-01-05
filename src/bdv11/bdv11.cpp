#include "trace/trace.h"
#include "bdv11.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <string.h>
#include <stdlib.h>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;
using std::bind;
using std::placeholders::_1;
using std::shared_ptr;

BDV11::BDV11 (Qbus *bus)
	:
	PDP11Peripheral (bus),
	pcr {0},
	scratch {0},
	option {0},
	display {0},
	ltc {0},
	switchRegister_ {BDV11_SWITCH},
	time {0.0},
	ltcThread_ {thread(&BDV11::tick, this)},
	running_ {true}
{
	bus_->BINIT().subscribe (bind (&BDV11::BINITReceiver, this, _1));
	bus_->BPOK().subscribe (bind (&BDV11::BPOKReceiver, this, _1));
}


BDV11::BDV11 (Qbus *bus, shared_ptr<BDV11Config> bdv11Config)
	:
	BDV11 (bus)
{
	switchRegister_ = switchRegisterProgramSelection (bdv11Config);
	switchRegister_ |= switchRegisterBootDevice (bdv11Config);

	romUsed_ = romToUse (bdv11Config);
}

BDV11::~BDV11 ()
{
	running_ = false;
	ltcThread_.join();
}

u16 BDV11::switchRegisterProgramSelection (shared_ptr<BDV11Config> bdv11Config)
{
	u16 switchRegister {0};

	if (bdv11Config->cpuTests)
		switchRegister |= BDV11_CPU_TEST;
	if (bdv11Config->memoryTests)
		switchRegister |= BDV11_MEM_TEST;
	if (bdv11Config->decnetBoot)
		switchRegister |= BDV11_DECNET;
	if (bdv11Config->consoleDialog)
		switchRegister |= BDV11_DIALOG;

	return switchRegister;
}

u16 BDV11::switchRegisterBootDevice (shared_ptr<BDV11Config> bdv11Config)
{
	return bootDevices.find (bdv11Config->bootDevice)->second;
}

BDV11::ROMimage* BDV11::romToUse (shared_ptr<BDV11Config> bdv11Config)
{
	return &availableROMs.find (bdv11Config->bootROM)->second;
}

// A block of 256 LSI-11 bus addresses is reserved for use in addressing ROM
// locations on the BDV11 module. This block resides in the upper 4K address
// bank (28K-32K), which is normally used for peripheral-device addressing,
// and consists of byte addresses 173000-173776 (512 byte addresses correspond
// to 256 word addresses in the LSI-11 addressing scheme).
//
// The BDV11 logic enables all 2048 locations in a selected 2K ROM (or 1024
// locations in a 1K ROM) to be addressed by just these 256 bus addresses.
// The logic includes a page control register (PCR) at bus address 177520;
// the contents of this read/write register determine the specific ROM
// location that is accessed when one of the 256 bus addresses is placed on
// the BDAL lines. The PCR is loaded with "page" information, i.e., the PCR
// contents point to one of 16 (or one of 8) 128-word pages in the seiected
// ROM (16 pages X 128 words = 2048 words). (EK-BDV11-TM-001)
// 
// A 2K ROM comprises contents for two pages, the first (lowest) page contains
// the contents for addresses 173000 - 173376 and the second (highest) page
// contains the contents for addresses 173400 - 173777.
u16 BDV11::getWordLow (u16 word)
{
	u16 page = pcr & 0xFF;
	if (page < 0x10)
	{
		u16 romword = page * 0200 + word;
		return (*romUsed_)[romword];
	}
	else
	{
		return 0177777;
	}
}

u16 BDV11::getWordHigh (u16 word)
{
	u16 page = (pcr >> 8) & 0xFF;
	if (page < 0x10)
	{
		u16 romword = page * 0200 + word;
		return (*romUsed_)[romword];
	} 
	else 
	{
		return 0233;
	}
}

void BDV11::memoryDump (u16 pcr, int hi)
{
	const u16* data;
	u16 addr;
	u16 len = 0400;
	if (hi)
	{
		addr = 0173400;
		u16 page = (pcr >> 8) & 0xFF;
		if (page < 0x10) 
		{
			u16 romword = page * 0200;
			data = &(*romUsed_)[romword];
		} 
		else 
		{
			data = NULL;
		}
	} 
	else 
	{
		addr = 0173000;
		u16 page = pcr & 0xFF;
		if (page < 0x10) 
		{
			u16 romword = page * 0200;
			data = &(*romUsed_)[romword];
		} 
		else 
		{
			data = NULL;
		}
	}

	if (data) 
		trace.memoryDump ((u8*) data, addr, len);
	else 
	{
		u8 buf[0400];
		memset (buf, 0xFF, sizeof(buf));
		trace.memoryDump ((u8*) data, addr, len);
	}
}

StatusCode BDV11::read (BusAddress<> busAddress, u16 *destAddress)
{
	switch (busAddress.registerAddress ()) 
	{
		case PCR:
			// Page Control Register
			*destAddress = pcr;
			break;

		case RWR:
			// Read/Write Register
			*destAddress = scratch;
			break;

		case SDR:
			// Switch and Display Register
			*destAddress = switchRegister_;
			break;

		case LKS:
			// Line Clock Status Register. According to the BDV11 technical
			// manual (EK-BDV11-TM-001) this is a write-only register.
			*destAddress = ltc;
			break;

		default:
			if (busAddress.registerAddress () >= 0173000 && 
				busAddress.registerAddress () < 0173400)
			{
				*destAddress = getWordLow ((busAddress.registerAddress () - 0173000) / 2);
				break;
			} 
			else if (busAddress.registerAddress () >= 0173400 &&
				busAddress.registerAddress () <= 0173776) 
			{
				*destAddress = getWordHigh ((busAddress.registerAddress () - 0173400) / 2);
				break;
			}
			else
				return StatusCode::NonExistingMemory;
	}
	return StatusCode::OK;
}

StatusCode BDV11::writeWord (BusAddress<> busAddress, u16 value)
{
	switch (busAddress.registerAddress ())
	{
		case PCR:
			// Page Control Register
			// Record new memory content in trace */
			if (value != pcr)
			{
				if ((value & 0xFF) == (pcr & 0xFF)) 
				{
					memoryDump (value, 1);
				} 
				else if ((value & 0xFF00) == (pcr & 0xFF00)) 
				{
					memoryDump (value, 0);
				} 
				else 
				{
					memoryDump (value, 0);
					memoryDump (value, 1);
				}
			}
			pcr = value;
			break;

		case RWR:
			// Read/Write Register
			scratch = value;
			break;

		case SDR:
			// Switch and Display Register
			display = value;
			break;

		case LKS:
			// Line Clock Status register
			writeLKS (value);
			break;
	}

	return StatusCode::OK;
}

// Write the Line Time Clock Status Register.
void BDV11::writeLKS (u16 value)
{
	ltc = value & LKS_IE;
}

// As the BDV11 will only be accessed by means of unmapped (16-bit) addresses
// the given bus address can be compared directly with the BDV11's device
// addresses.
bool BDV11::responsible (BusAddress<> busAddress)
{
	// The BDV11 registers are word or byte addressable
	switch (busAddress.registerAddress () & 0177776)
	{
		case PCR:
			// Page Control Register
		case RWR:
			// Read/Write Register
		case SDR:
			// Switch and Display Register
		case LKS:
			// Line Clock Status Register
			return true;
		default:
			return (busAddress.registerAddress () >= 0173000 && 
				busAddress.registerAddress () <= 0173776) ? true : false;
	}
}

// On assertion of the BINIT signal initialize the device.
void BDV11::BINITReceiver (bool signalValue)
{
	if (signalValue)
		reset ();
}

// On power down switch off the line time clock.
void BDV11::BPOKReceiver (bool signalValue)
{
	if (!signalValue)
		ltc = 0;
}

// The page https://gunkies.org/wiki/BDV11_Bus_Terminator/ROM reports the
// following issue with the line time clock: "Once the KW11-L on a BDV11 has
// been enabled, RESET/etc do not disable it, unlike every other device in the
// PDP-11 universe! (This is apparently because they were short a QBUS
// receiver for the BINIT bus line, and didn't want to add another complete
// chip to be able to monitor it.)
// So if you start some operating system which enables the KW11-L (e.g. UNIX),
// and then try and re-boot it by halting the machine and using the 'Go'
// command to ODT, you'll probably get a clock interrupt in the process of
// booting, which will trash it.
// The only thing that clears it is either a manual disable (write 0 to the
// KW11-L's CSR); something which toggles the BDCOK bus line - e.g. using the
// BDV11's on-board RESTART switch, or a front-panel restart switch; or power
// cycling the machine".
//
void BDV11::reset ()
{
	pcr = 0;
	scratch = 0;
	display = 0;
	ltc = 0;

	// Clear possibly pending interrupts
	bus_->clearInterrupt (TrapPriority::BR6, 9, 0);

	memoryDump (pcr, 0);
	memoryDump (pcr, 1);
}

// Generate an Event interrupt on every clock tick. To get accurate timing
// the high_resolution_clock is used with an interval specified in
// nanoseconds.
//
// The interrupt behaviour of the LTC in the BDV11 is curious. Disabling
// interrupts in the CSR doesn't clear pending interrupts but indicates
// that in the next clock tick pending interrupts are to be cleared. This
// behaviour isn't documented but can be deduced from diagnostics JKDBD0
// test 626 and JKDIB0 test 50.
//
void BDV11::tick()
{
	AlarmClock alarmClock {};
	SimulatorClock::time_point nextWakeup {SimulatorClock::now()};
	constexpr SimulatorClock::duration cycleTime {nanoseconds (1000000000 / LTC_RATE)};

	while (running_)
	{
		// Check the line time clock (LTC) is enabled
		if (ltc & LKS_IE)
			bus_->setInterrupt (TrapPriority::BR6, 9, 0, 0100);
		else
			// Clear possibly pending interrupts
			bus_->clearInterrupt (TrapPriority::BR6, 9, 0);
	
		nextWakeup += cycleTime;
		alarmClock.sleepUntil (nextWakeup);
	}
}
